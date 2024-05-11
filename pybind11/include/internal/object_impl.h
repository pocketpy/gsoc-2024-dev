#pragma once

#include "builtins.h"

#define PYBIND11_UNARY_OPERATOR(op, name)                                      \
    template <typename Derived>                                                \
    object object_api<Derived>::operator op () const                           \
    {                                                                          \
        return reinterpret_borrow<object>(                                     \
            vm->call(vm->py_op(name), derived().ptr()));                       \
    }

#define PYBIND11_BINARY_OPERATOR(op, name)                                     \
    template <typename Derived>                                                \
    object object_api<Derived>::operator op (const object_api & other)         \
    {                                                                          \
        return reinterpret_borrow<object>(vm->call(vm->py_op(name),            \
                                                   derived().ptr(),            \
                                                   other.derived().ptr()));    \
    }

#define PYBIND11_BINARY_CONST_OPERATOR(op, name)                               \
    template <typename Derived>                                                \
    object object_api<Derived>::operator op (const object_api & other) const   \
    {                                                                          \
        return reinterpret_borrow<object>(vm->call(vm->py_op(name),            \
                                                   derived().ptr(),            \
                                                   other.derived().ptr()));    \
    }

namespace pybind11
{

    PYBIND11_UNARY_OPERATOR(-, "neg");
    PYBIND11_UNARY_OPERATOR(~, "invert");

    PYBIND11_BINARY_CONST_OPERATOR(+, "add");
    PYBIND11_BINARY_CONST_OPERATOR(-, "sub");
    PYBIND11_BINARY_CONST_OPERATOR(*, "mul");
    PYBIND11_BINARY_CONST_OPERATOR(/, "truediv");
    PYBIND11_BINARY_CONST_OPERATOR(%, "mod");
    PYBIND11_BINARY_CONST_OPERATOR(|, "or_");
    PYBIND11_BINARY_CONST_OPERATOR(&, "and_");
    PYBIND11_BINARY_CONST_OPERATOR(^, "xor");
    PYBIND11_BINARY_CONST_OPERATOR(<<, "lshift");
    PYBIND11_BINARY_CONST_OPERATOR(>>, "rshift");

    PYBIND11_BINARY_OPERATOR(+=, "iadd");
    PYBIND11_BINARY_OPERATOR(-=, "isub");
    PYBIND11_BINARY_OPERATOR(*=, "imul");
    PYBIND11_BINARY_OPERATOR(/=, "itruediv");
    PYBIND11_BINARY_OPERATOR(%=, "imod");
    PYBIND11_BINARY_OPERATOR(|=, "ior");
    PYBIND11_BINARY_OPERATOR(&=, "iand");
    PYBIND11_BINARY_OPERATOR(^=, "ixor");
    PYBIND11_BINARY_OPERATOR(<<=, "ilshift");
    PYBIND11_BINARY_OPERATOR(>>=, "irshift");

    template <typename Derived>
    template <return_value_policy policy, typename... Args>
    object object_api<Derived>::operator() (Args&&... args) const
    {
        return reinterpret_borrow<object>(
            vm->call(derived().ptr(),
                     _cast(std::forward<Args>(args)).ptr()...));
    }

    template <typename Derived>
    args_proxy object_api<Derived>::operator* () const
    {
        // TODO: support args_proxy
    }

    // TODO: other builtin magic methods

    template <typename Policy>
    class accessor : public object_api<accessor<Policy>>
    {
        using key_type = typename Policy::key_type;

    public:
        accessor(handle obj, key_type key) : obj(obj), key(std::move(key)) {}

        accessor(const accessor&) = default;
        accessor(accessor&&) noexcept = default;

        // accessor overload required to override default assignment operator
        // (templates are not allowed to replace default compiler-generated
        // assignments).
        void operator= (const accessor& a) &&
        {
            std::move(*this).operator= (handle(a));
        }

        void operator= (const accessor& a) & { operator= (handle(a)); }

        template <typename T>
        void operator= (T&& value) &&
        {
            Policy::set(obj, key, pybind11::cast(std::forward<T>(value)));
        }

        template <typename T>
        void operator= (T&& value) &
        {
            get_cache() = pybind11::cast(std::forward<T>(value));
        }

        // NOLINTNEXTLINE(google-explicit-constructor)
        operator object () const { return get_cache(); }

        pkpy::PyObject* ptr() const { return get_cache().ptr(); }

        template <typename T>
        T cast() const
        {
            return get_cache().template cast<T>();
        }

    private:
        static object ensure_object(object&& o) { return std::move(o); }

        static object ensure_object(handle h) {}

        object& get_cache() const
        {
            if(!cache)
            {
                cache = Policy::get(obj, key);
            }
            return cache;
        }

    private:
        handle obj;
        key_type key;
        mutable object cache;
    };

    namespace accessor_policies
    {
        struct obj_attr
        {
            using key_type = object;

            static object get(handle obj, handle key)
            {
                return getattr(obj, key);
            }

            static void set(handle obj, handle key, handle val)
            {
                setattr(obj, key, val);
            }
        };

        struct str_attr
        {
            using key_type = const char*;

            static object get(handle obj, const char* key)
            {
                return getattr(obj, key);
            }

            static void set(handle obj, const char* key, handle val)
            {
                setattr(obj, key, val);
            }
        };

        struct generic_item
        {
            using key_type = object;

            static object get(handle obj, handle key)
            {
                pkpy::PyObject* result =
                    vm->call_method(obj.ptr(), pkpy::__getitem__, key.ptr());
                return reinterpret_borrow<object>(result);
            }

            static void set(handle obj, handle key, handle val)
            {
                vm->call_method(obj.ptr(),
                                pkpy::__setitem__,
                                key.ptr(),
                                val.ptr());
            }
        };

    }  // namespace accessor_policies

    template <typename Derived>
    item_accessor object_api<Derived>::operator[] (handle key) const
    {
        return item_accessor(derived(), reinterpret_borrow<object>(key));
    }

    template <typename Derived>
    item_accessor object_api<Derived>::operator[] (object&& key) const
    {
        return item_accessor(derived(), std::move(key));
    }

    template <typename Derived>
    item_accessor object_api<Derived>::operator[] (const char* key) const
    {
        return item_accessor(derived(), pybind11::str(key));
    }

    template <typename Derived>
    obj_attr_accessor object_api<Derived>::attr(handle key) const
    {
        return obj_attr_accessor(derived(), key);
    }

    template <typename Derived>
    obj_attr_accessor object_api<Derived>::attr(object&& key) const
    {
        return obj_attr_accessor(derived(), std::move(key));
    }

    template <typename Derived>
    str_attr_accessor object_api<Derived>::attr(const char* key) const
    {
        return str_attr_accessor(derived(), key);
    }

    // template <typename Derived>
    // iterator object_api<Derived>::begin() const
    //{
    //     return iterator(derived());
    // }
    //
    // template <typename Derived>
    // iterator object_api<Derived>::end() const
    //{
    //    return iterator(derived(), true);
    //}
}  // namespace pybind11

#undef PYBIND11_UNARY_OPERATOR
#undef PYBIND11_BINARY_OPERATOR
#undef PYBIND11_BINARY_CONST_OPERATOR
