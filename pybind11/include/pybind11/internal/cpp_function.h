#pragma once
#include "cast.h"

namespace pybind11 {

template <std::size_t Nurse, std::size_t... Patients>
struct keep_alive {};

template <typename T>
struct call_guard {
    static_assert(std::is_default_constructible_v<T>, "call_guard must be default constructible");
};

// append the overload to the beginning of the overload list
struct prepend {};

template <typename... Args>
struct init {};

// TODO: support more customized tags
// struct kw_only {};
//
// struct pos_only {};
//
// struct default_arg {};
//
// struct arg {
//    const char* name;
//    const char* description;
// };
//
// struct default_arg {
//    const char* name;
//    const char* description;
//    const char* value;
// };
}  // namespace pybind11

namespace pybind11::impl {

struct arguments_info {
    int argc = 0;
    int args = -1;
    int kwargs = -1;
};

template <typename Args>
struct helper;

template <typename... Args>
struct helper<std::tuple<Args...>> {
    constexpr static auto parse_arguments() {
        arguments_info info;
        info.argc = sizeof...(Args);
        constexpr auto args_ = types_count_v<args, Args...>;
        if constexpr(args_ == 1) {
            info.args = type_index_v<args, Args...>;
        } else if constexpr(args_ > 1) {
            info.args = -2;
        }

        constexpr auto kwargs_ = types_count_v<kwargs, Args...>;
        if constexpr(kwargs_ == 1) {
            info.kwargs = type_index_v<kwargs, Args...>;
        } else if constexpr(kwargs_ > 1) {
            info.kwargs = -2;
        }

        return info;
    }
};

template <typename Fn,
          typename Extra,
          typename Args = callable_args_t<std::decay_t<Fn>>,
          typename IndexSequence = std::make_index_sequence<std::tuple_size_v<Args>>>
struct generator;

class function_record {
    union {
        void* data;
        char buffer[16];
    };

    // TODO: optimize the function_record size to reduce memory usage
    const char* name;
    function_record* next;
    void (*destructor)(function_record*);
    return_value_policy policy = return_value_policy::automatic;
    handle (*wrapper)(function_record&, pkpy::ArgsView, bool convert, handle parent);

    template <typename Fn, typename Extra, typename Args, typename IndexSequence>
    friend struct generator;

public:
    template <typename Fn, typename... Extras>
    function_record(Fn&& f, const char* name, const Extras&... extras) : name(name), next(nullptr) {

        if constexpr(std::is_trivially_copyable_v<Fn> && sizeof(Fn) <= sizeof(buffer)) {
            new (buffer) auto(std::forward<Fn>(f));
            destructor = [](function_record* self) {
                reinterpret_cast<Fn*>(self->buffer)->~Fn();
            };
        } else {
            data = new auto(std::forward<Fn>(f));
            destructor = [](function_record* self) {
                delete static_cast<Fn*>(self->data);
            };
        }

        using Generator = generator<std::decay_t<Fn>, std::tuple<Extras...>>;
        Generator::initialize(*this, extras...);
        wrapper = Generator::generate();
    }

    function_record(const function_record&) = delete;

    function_record(function_record&& other) noexcept :
        name(other.name), next(other.next), destructor(other.destructor), policy(other.policy), wrapper(other.wrapper) {
        std::memcpy(buffer, other.buffer, sizeof(buffer));
        other.destructor = nullptr;
    }

    ~function_record() {
        if(destructor) { destructor(this); }
    }

    template <typename Fn>
    auto& cast() {
        if constexpr(std::is_trivially_copyable_v<Fn> && sizeof(Fn) <= sizeof(buffer)) {
            return *reinterpret_cast<Fn*>(buffer);
        } else {
            return *static_cast<Fn*>(data);
        }
    }

    void append(function_record* record) {
        function_record* p = this;
        while(p->next != nullptr) {
            p = p->next;
        }
        p->next = record;
    }

    handle operator() (pkpy::ArgsView view) {
        function_record* p = this;
        // foreach function record and call the function with not convert
        while(p != nullptr) {
            handle result = p->wrapper(*this, view, false, {});
            if(result) { return result; }
            p = p->next;
        }

        p = this;
        // foreach function record and call the function with convert
        while(p != nullptr) {
            handle result = p->wrapper(*this, view, true, {});
            if(result) { return result; }
            p = p->next;
        }

        vm->TypeError("no matching function found");
    }
};

template <typename Fn, std::size_t... Is, typename... Args>
handle invoke(Fn&& fn,
              std::index_sequence<Is...>,
              std::tuple<type_caster<Args>...>& casters,
              return_value_policy policy,
              handle parent) {
    using underlying_type = std::decay_t<Fn>;
    using ret = callable_return_t<underlying_type>;

    // if the return type is void, return None
    if constexpr(std::is_void_v<ret>) {
        // resolve the member function pointer
        if constexpr(std::is_member_function_pointer_v<underlying_type>) {
            [&](class_type_t<underlying_type>& self, auto&... args) {
                (self.*fn)(args...);
            }(std::get<Is>(casters).value...);
        } else {
            fn(std::get<Is>(casters).value...);
        }
        return vm->None;
    } else {
        // resolve the member function pointer
        if constexpr(std::is_member_function_pointer_v<remove_cvref_t<Fn>>) {
            return type_caster<ret>::cast(
                [&](class_type_t<underlying_type>& self, auto&... args) {
                    return (self.*fn)(args...);
                }(std::get<Is>(casters).value...),
                policy,
                parent);
        } else {
            return type_caster<ret>::cast(fn(std::get<Is>(casters).value...), policy, parent);
        }
    }
}

template <typename Fn, typename... Args, std::size_t... Is, typename... Extras>
struct generator<Fn, std::tuple<Extras...>, std::tuple<Args...>, std::index_sequence<Is...>> {
    static void initialize(function_record& record, const Extras&... extras) {}

    static auto generate() {
        return +[](function_record& self, pkpy::ArgsView view, bool convert, handle parent) {
            // FIXME:
            // Temporarily, args and kwargs must be at the end of the arguments list
            // Named arguments are not supported yet
            constexpr bool has_args = types_count_v<args, remove_cvref_t<Args>...> != 0;
            constexpr bool has_kwargs = types_count_v<kwargs, remove_cvref_t<Args>...> != 0;
            constexpr std::size_t count = sizeof...(Args) - has_args - has_kwargs;

            handle stack[sizeof...(Args)];

            // initialize the stack

            if(!has_args && (view.size() != count)) { return handle(); }

            if(has_args && (view.size() < count)) { return handle(); }

            for(std::size_t i = 0; i < count; ++i) {
                stack[i] = view[i];
            }

            // pack the args and kwargs
            if constexpr(has_args) {
                const auto n = view.size() - count;
                pkpy::PyVar var = vm->new_object<pkpy::Tuple>(vm->tp_tuple, n);
                auto& tuple = var.obj_get<pkpy::Tuple>();
                for(std::size_t i = 0; i < n; ++i) {
                    tuple[i] = view[count + i];
                }
                stack[count] = var;
            }

            if constexpr(has_kwargs) {
                const auto n = vm->s_data._sp - view.end();
                pkpy::PyVar var = vm->new_object<pkpy::Dict>(vm->tp_dict);
                auto& dict = var.obj_get<pkpy::Dict>();

                for(std::size_t i = 0; i < n; i += 2) {
                    pkpy::i64 index = pkpy::_py_cast<pkpy::i64>(vm, view[count + i]);
                    pkpy::PyVar str = vm->new_object<pkpy::Str>(vm->tp_str, pkpy::StrName(index).sv());
                    dict.set(vm, str, view[count + i + 1]);
                }

                stack[count + has_args] = var;
            }

            // check if all the arguments are not valid
            for(std::size_t i = 0; i < sizeof...(Args); ++i) {
                if(!stack[i]) { return handle(); }
            }

            // ok, all the arguments are valid, call the function
            std::tuple<type_caster<Args>...> casters;

            // check type compatibility
            if(((std::get<Is>(casters).load(stack[Is], convert)) && ...)) {
                return invoke(self.cast<Fn>(), std::index_sequence<Is...>{}, casters, self.policy, parent);
            }

            return handle();
        };
    }
};

// class cpp_function : public function {
// public:
//     template <typename Fn, typename... Extras>
//     cpp_function(Fn&& f, const Extras&... extras) {
//         pkpy::any userdata = function_record(std::forward<Fn>(f), "anonymous", extras...);
//         m_ptr = vm->bind_func(nullptr, "", -1, _wrapper, std::move(userdata));
//     }
// };

inline auto _wrapper(pkpy::VM* vm, pkpy::ArgsView view) {
    auto& record = unpack<function_record>(view);
    return record(view).ptr();
}

template <typename Fn, typename... Extras>
handle bind_function_impl(const handle& obj, const char* name, Fn&& fn, pkpy::BindType type, const Extras&... extras) {
    // do not use cpp_function directly to avoid unnecessary reference count change
    pkpy::PyVar var = obj.ptr();
    pkpy::PyVar callable = var->attr().try_get(name);

    // if the function is not bound yet, bind it
    if(!callable) {
        auto record = function_record(std::forward<Fn>(fn), name, extras...);
        void* data = add_capsule(std::move(record));
        callable = vm->bind_func(var.get(), name, -1, _wrapper, data);
    } else {
        auto& userdata = callable.obj_get<pkpy::NativeFunc>()._userdata;
        function_record* record = new function_record(std::forward<Fn>(fn), name, extras...);

        constexpr bool is_prepend = (types_count_v<prepend, Extras...> != 0);
        if constexpr(is_prepend) {
            // if prepend is specified, append the new record to the beginning of the list
            function_record* last = (function_record*)userdata.data;
            userdata.data = record;
            record->append(last);
        } else {
            // otherwise, append the new record to the end of the list
            function_record* last = (function_record*)userdata.data;
            last->append(record);
        }
    }
    return callable;
}

template <typename Fn, typename... Extras>
handle bind_function(const handle& obj, const char* name, Fn&& fn, pkpy::BindType type, const Extras&... extras) {
    using Args = callable_args_t<std::decay_t<Fn>>;
    constexpr auto arguments_info = helper<Args>::parse_arguments();

    constexpr auto argc = arguments_info.argc;
    constexpr auto args = arguments_info.args;
    constexpr auto kwargs = arguments_info.kwargs;

    constexpr bool _0 = args == -2 || kwargs == -2;
    static_assert(!_0, "args or kwargs can occur at most once");

    constexpr bool _1 = args >= 0 && kwargs >= 0 && args > kwargs;
    static_assert(!_1, "args must be before kwargs");

    constexpr bool _2 = kwargs >= 0 && kwargs != argc - 1;
    static_assert(!_2, "kwargs must be the last argument");

    if constexpr(!(_0 || _1 || _2)) { return bind_function_impl(obj, name, std::forward<Fn>(fn), type, extras...); }
}

template <typename Getter>
pkpy::PyVar getter_wrapper(pkpy::VM* vm, pkpy::ArgsView view) {
    handle result = vm->None;
    auto& getter = unpack<Getter>(view);
    constexpr auto policy = return_value_policy::reference_internal;

    if constexpr(std::is_member_pointer_v<Getter>) {
        using Self = class_type_t<Getter>;
        auto& self = handle(view[0])._as<instance>()._as<Self>();

        if constexpr(std::is_member_object_pointer_v<Getter>) {
            // specialize for pointer to data member
            result = cast(self.*getter, policy, view[0]);
        } else {
            // specialize for pointer to member function
            result = cast((self.*getter)(), policy, view[0]);
        }
    } else {
        // specialize for function pointer and lambda
        using Self = remove_cvref_t<std::tuple_element_t<0, callable_args_t<Getter>>>;
        auto& self = handle(view[0])._as<instance>()._as<Self>();

        result = cast(getter(self), policy, view[0]);
    }

    return result.ptr();
}

template <typename Setter>
pkpy::PyVar setter_wrapper(pkpy::VM* vm, pkpy::ArgsView view) {
    auto& setter = unpack<Setter>(view);

    if constexpr(std::is_member_pointer_v<Setter>) {
        using Self = class_type_t<Setter>;
        auto& self = handle(view[0])._as<instance>()._as<Self>();

        if constexpr(std::is_member_object_pointer_v<Setter>) {
            // specialize for pointer to data member
            type_caster<member_type_t<Setter>> caster;
            if(caster.load(view[1], true)) {
                self.*setter = caster.value;
                return vm->None;
            }
        } else {
            // specialize for pointer to member function
            type_caster<std::tuple_element_t<1, callable_args_t<Setter>>> caster;
            if(caster.load(view[1], true)) {
                (self.*setter)(caster.value);
                return vm->None;
            }
        }
    } else {
        // specialize for function pointer and lambda
        using Self = remove_cvref_t<std::tuple_element_t<0, callable_args_t<Setter>>>;
        auto& self = handle(view[0])._as<instance>()._as<Self>();

        type_caster<std::tuple_element_t<1, callable_args_t<Setter>>> caster;
        if(caster.load(view[1], true)) {
            setter(self, caster.value);
            return vm->None;
        }
    }

    vm->TypeError("Unexpected argument type");
}

template <typename Getter, typename Setter, typename... Extras>
handle bind_property(const handle& obj, const char* name, Getter&& getter_, Setter&& setter_, const Extras&... extras) {
    handle getter = none();
    handle setter = none();
    using Wrapper = pkpy::PyVar (*)(pkpy::VM*, pkpy::ArgsView);

    constexpr auto create = [](Wrapper wrapper, int argc, auto&& f) {
        if constexpr(need_host<remove_cvref_t<decltype(f)>>) {
            // otherwise, store it in the type_info
            void* data = add_capsule(std::forward<decltype(f)>(f));
            // store the index in the object
            return vm->new_object<pkpy::NativeFunc>(vm->tp_native_func, wrapper, argc, data);
        } else {
            // if the function is trivially copyable and the size is less than 16 bytes, store it in the object directly
            return vm->new_object<pkpy::NativeFunc>(vm->tp_native_func, wrapper, argc, f);
        }
    };

    getter = create(impl::getter_wrapper<std::decay_t<Getter>>, 1, std::forward<Getter>(getter_));

    if constexpr(!std::is_same_v<Setter, std::nullptr_t>) {
        setter = create(impl::setter_wrapper<std::decay_t<Setter>>, 2, std::forward<Setter>(setter_));
    }

    handle property = pybind11::property(getter, setter);
    setattr(obj, name, property);
    return property;
}

}  // namespace pybind11::impl
