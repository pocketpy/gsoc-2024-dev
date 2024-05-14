#pragma once

#include "pytypes.h"

namespace pybind11
{
    template <typename T>
    bool isinstance(handle obj);

    template <typename T, typename = void>
    struct type_caster;

    template <>
    struct type_caster<void>
    {
        template <typename T>
        static handle cast(T&&, return_value_policy, handle)
        {
            return vm->None;
        }
    };

    template <>
    struct type_caster<bool>
    {
        bool value;

        bool load(handle src, bool)
        {
            // TODO: numpy bool
            if(isinstance<pybind11::bool_>(src))
            {
                value = pkpy::_py_cast<bool>(vm, src.ptr());
                return true;
            }

            return false;
        }

        static handle cast(bool src, return_value_policy, handle)
        {
            return src ? vm->True : vm->False;
        }
    };

    template <typename T>
    struct type_caster<T, std::enable_if_t<pkpy::is_integral_v<T>>>
    {
        T value;

        bool load(handle src, bool convert)
        {
            if(isinstance<pybind11::int_>(src))
            {
                value = pkpy::_py_cast<T>(vm, src.ptr());
                return true;
            }

            return false;
        }

        static handle cast(T src, return_value_policy, handle) { return pkpy::py_var(vm, src); }
    };

    template <typename T>
    struct type_caster<T, std::enable_if_t<pkpy::is_floating_point_v<T>>>
    {
        T value;

        bool load(handle src, bool convert)
        {
            if(isinstance<pybind11::float_>(src))
            {
                value = pkpy::_py_cast<T>(vm, src.ptr());
                return true;
            }

            if(convert && isinstance<pybind11::int_>(src))
            {
                value = pkpy::_py_cast<int64_t>(vm, src.ptr());
                return true;
            }

            return false;
        }

        static handle cast(T src, return_value_policy, handle) { return pkpy::py_var(vm, src); }
    };

    // FIXME: support other kinds of string
    template <typename T>
    struct type_caster<T, std::enable_if_t<std::is_same_v<T, std::string>>>
    {
        std::string value;

        bool load(handle src, bool)
        {
            if(isinstance<pybind11::str>(src))
            {
                value = pkpy::_py_cast<std::string>(vm, src.ptr());
                return true;
            }

            return false;
        }

        static handle cast(const std::string& src, return_value_policy, handle)
        {
            return pkpy::py_var(vm, src);
        }
    };

    template <typename T>
    struct type_caster<T, std::enable_if_t<is_pyobject_v<T>>>
    {
        T value;

        bool load(handle src, bool)
        {
            if(isinstance<T>(src))
            {
                value = reinterpret_borrow<T>(src);
                return true;
            }

            return false;
        }

        template <typename U>
        static handle cast(U&& src, return_value_policy, handle)
        {
            return std::forward<U>(src);
        }
    };

    template <typename T, typename>
    struct type_caster
    {
        T* value = nullptr;

        bool load(handle src, bool convert)
        {
            if(isinstance<T>(src))
            {
                auto& i = _builtin_cast<instance>(src);
                value = &i.cast<T>();
                return true;
            }

            return false;
        }

        template <typename U>
        static handle cast(U&& value, return_value_policy policy, handle parent)
        {
            // TODO: support implicit cast
            bool existed = vm->_cxx_typeid_map.find(typeid(T)) != vm->_cxx_typeid_map.end();
            if(existed)
            {
                auto type = vm->_cxx_typeid_map[typeid(T)];
                return instance::create(std::forward<U>(value), type, policy, parent.ptr());
            }
            return nullptr;
        }
    };

    template <typename Caster>
    auto& value_of_caster(Caster& caster)
    {
        if constexpr(std::is_pointer_v<decltype(caster.value)>)
        {
            return *caster.value;
        }
        else
        {
            return caster.value;
        }
    }
}  // namespace pybind11
