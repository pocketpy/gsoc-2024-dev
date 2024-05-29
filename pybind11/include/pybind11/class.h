#pragma once

#include "cpp_function.h"

namespace pybind11 {

    class module : public object {
    public:
    };

    // TODO:
    // 1. inheritance
    // 2. virtual function
    // 3. factory function

    template <typename T, typename... Others>
    class class_ : public type {
    public:
        template <typename... Args>
        class_(const handle& scope, const char* name, Args&&... args) {
            pkpy::PyVar mod = scope.ptr();
            m_ptr = vm->new_type_object(mod,
                                        name,
                                        vm->tp_object,
                                        false,
                                        pkpy::PyTypeInfo::Vt::get<T>());

            mod->attr().set(name, m_ptr);
            vm->_cxx_typeid_map[typeid(T)] = _builtin_cast<pkpy::Type>(m_ptr);
            inc_ref();

            vm->bind_func(m_ptr, "__new__", -1, [](pkpy::VM* vm, pkpy::ArgsView args) {
                auto cls = _builtin_cast<pkpy::Type>(args[0]);
                return instance::create<T>(cls);
            });
        }

        /// bind constructor
        template <typename... Args, typename... Extra>
        class_& def(init<Args...>, const Extra&... extra) {
            if constexpr(!std::is_constructible_v<T, Args...>) {
                static_assert(std::is_constructible_v<T, Args...>, "Invalid constructor arguments");
            } else {
                bind_function(
                    *this,
                    "__init__",
                    [](T* self, Args... args) { new (self) T(args...); },
                    pkpy::BindType::DEFAULT,
                    extra...);
                return *this;
            }
        }

        /// bind member function
        template <typename Fn, typename... Extra>
        class_& def(const char* name, Fn&& f, const Extra&... extra) {
            using first = std::tuple_element_t<0, callable_args_t<remove_cvref_t<Fn>>>;
            constexpr bool is_first_base_of_v =
                std::is_reference_v<first> && std::is_base_of_v<T, remove_cvref_t<first>>;

            if constexpr(!is_first_base_of_v) {
                static_assert(
                    is_first_base_of_v,
                    "If you want to bind member function, the first argument must be the base class");
            } else {
                bind_function(*this, name, std::forward<Fn>(f), pkpy::BindType::DEFAULT, extra...);
            }

            return *this;
        }

        /// bind operators
        template <typename Operator, typename... Extras>
        class_& def(Operator op, const Extras&... extras) {
            op.bind(*this, extras...);
            return *this;
        }

        // TODO: factory function

        /// bind static function
        template <typename Fn, typename... Extra>
        class_& def_static(const char* name, Fn&& f, const Extra&... extra) {
            bind_function(*this, name, std::forward<Fn>(f), pkpy::BindType::STATICMETHOD, extra...);
            return *this;
        }

        template <typename MP, typename... Extras>
        class_& def_readwrite(const char* name, MP mp, const Extras&... extras) {
            if constexpr(!std::is_member_object_pointer_v<MP>) {
                static_assert(std::is_member_object_pointer_v<MP>,
                              "def_readwrite only supports pointer to data member");
            } else {
                bind_property(*this, name, mp, mp, extras...);
            }
            return *this;
        }

        template <typename MP, typename... Extras>
        class_& def_readonly(const char* name, MP mp, const Extras&... extras) {
            if constexpr(!std::is_member_object_pointer_v<MP>) {
                static_assert(std::is_member_object_pointer_v<MP>,
                              "def_readonly only supports pointer to data member");
            } else {
                bind_property(*this, name, mp, nullptr, extras...);
            }
            return *this;
        }

        template <typename Getter, typename Setter, typename... Extras>
        class_& def_property(const char* name, Getter&& g, Setter&& s, const Extras&... extras) {
            bind_property(*this, name, std::forward<Getter>(g), std::forward<Setter>(s), extras...);
            return *this;
        }

        template <typename Getter, typename... Extras>
        class_& def_property_readonly(const char* name, Getter&& mp, const Extras&... extras) {
            bind_property(*this, name, std::forward<Getter>(mp), nullptr, extras...);
            return *this;
        }

        template <typename Var, typename... Extras>
        class_& def_readwrite_static(const char* name, Var& mp, const Extras&... extras) {
            static_assert(
                dependent_false<Var>,
                "define static properties requires metaclass. This is a complex feature with few use cases, so it may never be implemented.");
            return *this;
        }

        template <typename Var, typename... Extras>
        class_& def_readonly_static(const char* name, Var& mp, const Extras&... extras) {
            static_assert(
                dependent_false<Var>,
                "define static properties requires metaclass. This is a complex feature with few use cases, so it may never be implemented.");
            return *this;
        }

        template <typename Getter, typename Setter, typename... Extras>
        class_&
            def_property_static(const char* name, Getter&& g, Setter&& s, const Extras&... extras) {
            static_assert(
                dependent_false<Getter>,
                "define static properties requires metaclass. This is a complex feature with few use cases, so it may never be implemented.");
            return *this;
        }
    };
}  // namespace pybind11
