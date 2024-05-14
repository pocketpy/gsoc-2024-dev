
#include <array>
#include <functional>
#include "object_impl.h"
#include "type_traits.h"

namespace pybind11
{
    struct arg
    {
        bool convert = true;
        handle value = nullptr;
        const char* name = nullptr;
    };

    template <std::size_t N = 1, typename KeepAlive = void, typename CallGuard = void>
    struct extras_info
    {
        bool prepend = false;
        bool has_default_arguments = false;
        bool has_args = false;
        bool has_kwargs = false;
        bool is_method = false;
        bool is_static = false;
        bool is_classmethod = false;
        std::array<arg, N> args;
        const char* doc = nullptr;
        return_value_policy policy = return_value_policy::automatic;
    };

    const inline std::string_view concat_signature(char* buffer,
                                                   const char* name,
                                                   std::size_t N,
                                                   bool has_args,
                                                   bool has_kwargs)
    {
        char* begin = buffer;
        std::size_t name_len = std::strlen(name);
        std::memcpy(buffer, name, name_len);
        buffer += name_len;

        *buffer++ = '(';

        for(std::size_t i = 0; i < N; ++i)
        {
            if(i != 0)
            {
                *buffer++ = ',';
            }
            int n = std::sprintf(buffer, "arg%zu", i);
            buffer += n;
        }

        if(has_args && !has_kwargs)
        {

            if(N == 0)
            {
                std::memcpy(buffer, "*args", 5);
                buffer += 5;
            }
            else
            {
                std::memcpy(buffer, ",*args", 7);
                buffer += 7;
            }
        }
        else if(!has_args && has_kwargs)
        {
            if(N == 0)
            {
                std::memcpy(buffer, "**kwargs", 8);
                buffer += 8;
            }
            else
            {
                std::memcpy(buffer, ",**kwargs", 10);
                buffer += 10;
            }
        }
        else if(has_args && has_kwargs)
        {
            if(N == 0)
            {
                std::memcpy(buffer, "*args,**kwargs", 14);
                buffer += 14;
            }
            else
            {
                std::memcpy(buffer, ", *args,**kwargs", 16);
                buffer += 16;
            }
        }

        *buffer++ = ')';
        *buffer = '\0';

        return {begin, static_cast<std::size_t>(buffer - begin)};
    }

    template <typename Fn,
              typename R = function_return_t<Fn>,
              typename Args = function_args_t<Fn>,
              typename IndexSequence = std::make_index_sequence<function_args_count<Fn>>>
    struct wrapper_gererator;

    template <typename Fn, typename R, typename... Args, std::size_t... Is>
    struct wrapper_gererator<Fn, R, std::tuple<Args...>, std::index_sequence<Is...>>
    {
        template <typename FunctionPointer>
        static auto wrap_function_pointer()
        {
            return +[](void* data, return_value_policy policy, bool convert, pkpy::ArgsView view)
                       -> pkpy::PyObject*
            {
                auto function_pointer = static_cast<FunctionPointer>(data);

                std::tuple<type_caster<Args>...> casters;
                const bool value = (std::get<Is>(casters).load(view[Is], convert) && ...);

                if(value)
                {

                    if constexpr(std::is_void_v<R>)
                    {
                        function_pointer(value_of_caster(std::get<Is>(casters))...);
                        return vm->None;
                    }
                    else
                    {
                        auto result = function_pointer(value_of_caster(std::get<Is>(casters))...);
                        return _cast(std::move(result), policy).ptr();
                    }
                }
                else
                {
                    return nullptr;
                }
            };
        }

        template <typename MemberPointer>
        static auto wrap_member_function_poiinter()
        {
            return +[](void* data, return_value_policy policy, bool convert, pkpy::ArgsView view)
                       -> pkpy::PyObject*
            {
                // first argument is self
                auto& self = cast<class_type_t<MemberPointer>&>(view[0]);
                auto& member_function_pointer = *static_cast<MemberPointer*>(data);

                std::tuple<type_caster<Args>...> casters;
                const bool value = (std::get<Is>(casters).load(view[Is + 1], convert) && ...);

                if(value)
                {

                    if constexpr(std::is_void_v<R>)
                    {
                        (self.*member_function_pointer)(value_of_caster(std::get<Is>(casters))...);
                        return vm->None;
                    }
                    else
                    {
                        auto result = (self.*member_function_pointer)(
                            value_of_caster(std::get<Is>(casters))...);
                        return _cast(std::move(result), policy).ptr();
                    }
                }
                else
                {
                    return nullptr;
                }
            };
        }

        template <typename Lambda>
        static auto wrap_lambda()
        {
            return +[](void* data, return_value_policy policy, bool convert, pkpy::ArgsView view)
                       -> pkpy::PyObject*
            {
                auto& lambda = *static_cast<Lambda*>(data);

                std::tuple<type_caster<Args>...> casters;
                const bool value = (std::get<Is>(casters).load(view[Is], convert) && ...);

                if(value)
                {
                    if constexpr(std::is_void_v<R>)
                    {
                        lambda(value_of_caster(std::get<Is>(casters))...);
                        return vm->None;
                    }
                    else
                    {
                        auto result = lambda(value_of_caster(std::get<Is>(casters))...);
                        return _cast(std::move(result), policy).ptr();
                    }
                }
                else
                {
                    return nullptr;
                }
            };
        }
    };

    using WrappedFn = pkpy::PyObject* (*)(void*, return_value_policy, bool, pkpy::ArgsView);

    class overload
    {
        void* data;
        return_value_policy policy;
        WrappedFn wrapper;
        std::vector<args> args;

    public:
        template <typename Fn, std::size_t N, typename KeepAlive, typename CallGuard>
        overload(Fn&& fn, const extras_info<N, KeepAlive, CallGuard>& extras)
        {
            using underlying_fn = std::decay_t<Fn>;
            if constexpr(pybind11::is_member_function_pointer_v<underlying_fn>)
            {
                using member_fn = member_type_t<underlying_fn>;
                using generator = wrapper_gererator<member_fn>;
                data = ::new auto(fn);
                wrapper = generator::template wrap_member_function_poiinter<underlying_fn>();
            }
            else if constexpr(pybind11::is_function_pointer_v<underlying_fn>)
            {
                using generator = wrapper_gererator<underlying_fn>;
                data = static_cast<void*>(fn);
                wrapper = generator::template wrap_function_pointer<underlying_fn>();
            }
            else if constexpr(is_functor_v<underlying_fn>)
            {
                using member_fn = member_type_t<decltype(&underlying_fn::operator())>;
                using generator = wrapper_gererator<member_fn>;
                data = ::new auto(std::forward<Fn>(fn));
                wrapper = generator::template wrap_lambda<underlying_fn>();
            }
            policy = extras.policy;
        }

        pkpy::PyObject* operator() (pkpy::VM* vm, pkpy::ArgsView view, bool convert)
        {
            return wrapper(data, policy, convert, view);
        }
    };

    constexpr inline pkpy::NativeFuncC fast_wrapper = [](pkpy::VM* vm, pkpy::ArgsView view)
    {
        auto& callable = pkpy::lambda_get_userdata<overload>(view.begin());
        return callable(vm, view, true);
    };

    // FIXME:
    class dispatcher
    {
        std::vector<overload> overloads;
        std::string error_msg;

    public:
        pkpy::PyObject* operator() (pkpy::VM* vm, pkpy::ArgsView view)
        {
            for(auto& overload: overloads)
            {
                auto result = overload(vm, view, false);
                if(result != nullptr)
                {
                    return result;
                }
            }

            for(auto& overload: overloads)
            {
                auto result = overload(vm, view, true);
                if(result != nullptr)
                {
                    return result;
                }
            }

            // TODO:
            throw std::exception(error_msg.c_str());
        }
    };

    constexpr pkpy::NativeFuncC dispatcher_wrapper = [](pkpy::VM* vm, pkpy::ArgsView view)
    {
        auto& callable = pkpy::lambda_get_userdata<dispatcher>(view.begin());
        return callable(vm, view);
    };

    template <typename Fn, std::size_t N, typename KeepAlive, typename CallGuard>
    void bind(handle obj,
              const char* name,
              Fn&& fn,
              const extras_info<N, KeepAlive, CallGuard>& extras)

    {
        pkpy::PyObject* func = obj.ptr()->attr().try_get(name);

        pkpy::BindType bind_type = extras.is_static        ? pkpy::BindType::STATICMETHOD
                                   : extras.is_classmethod ? pkpy::BindType::CLASSMETHOD
                                                           : pkpy::BindType::DEFAULT;

        // if do not have default arguments and the function is not bound
        if(!extras.has_default_arguments && func == nullptr)
        {
            char signature[1024];
            concat_signature(signature, name, N, extras.has_args, extras.has_kwargs);
            pkpy::any userdata = overload{std::forward<Fn>(fn), extras};
            auto _ = vm->bind(obj.ptr(), signature, fast_wrapper, std::move(userdata), bind_type);
        }
        else
        {
            //  rebind
            auto& native_func = _builtin_cast<pkpy::NativeFunc>(func);

            // use dispatcher
        }
    }

}  // namespace pybind11

