#pragma once

#include "instance.h"

namespace pybind11
{
    // foward declaration
    class handle;
    class object;
    class iterator;
    class type;
    class str;
    class bytes;
    class args_proxy;
    inline bool hasattr(handle obj, handle name);
    inline bool hasattr(handle obj, const char* name);
    inline object getattr(handle obj, handle name);
    inline object getattr(handle obj, const char* name);
    inline void setattr(handle obj, handle name, handle value);
    inline void setattr(handle obj, const char* name, handle value);

    template <typename Policy>
    class accessor;

    namespace accessor_policies
    {
        struct obj_attr;
        struct str_attr;
        struct generic_item;
    }  // namespace accessor_policies

    using obj_attr_accessor = accessor<accessor_policies::obj_attr>;
    using str_attr_accessor = accessor<accessor_policies::str_attr>;
    using item_accessor = accessor<accessor_policies::generic_item>;

    struct pyobject_tag
    {
    };

    // CRTP class used to inject method
    template <typename Derived>
    class object_api : public pyobject_tag
    {
    private:
        const Derived& derived() const { return static_cast<const Derived&>(*this); }

    public:
        iterator begin() const;

        iterator end() const;

        item_accessor operator[] (handle key) const;

        item_accessor operator[] (object&& key) const;

        item_accessor operator[] (const char* key) const;

        obj_attr_accessor attr(handle key) const;

        obj_attr_accessor attr(object&& key) const;

        str_attr_accessor attr(const char* key) const;

        args_proxy operator* () const;

        /// Check if the given item is contained within this object, i.e. ``item in obj``.
        template <typename T>
        bool contains(T&& item) const;

        template <return_value_policy policy = return_value_policy::automatic_reference, typename... Args>
        object operator() (Args&&... args) const;

        /// Equivalent to ``obj is None`` in Python.
        bool is_none() const { return derived().ptr() == vm->None; }

        /// Equivalent to ``obj is other`` in Python.
        bool is(const object_api& other) const { return derived().ptr() == other.derived().ptr(); }

        /// Equivalent to obj == other in Python
        // NOTE: Different from pybind11
        bool operator== (const object_api& other) const { return vm->py_eq(derived().ptr(), other.derived().ptr()); }

        bool operator!= (const object_api& other) const { return vm->py_ne(derived().ptr(), other.derived().ptr()); }

        bool operator< (const object_api& other) const { return vm->py_lt(derived().ptr(), other.derived().ptr()); }

        bool operator<= (const object_api& other) const { return vm->py_le(derived().ptr(), other.derived().ptr()); }

        bool operator> (const object_api& other) const { return vm->py_gt(derived().ptr(), other.derived().ptr()); }

        bool operator>= (const object_api& other) const { return vm->py_ge(derived().ptr(), other.derived().ptr()); }

        object operator- () const;
        object operator~() const;
        object operator+ (const object_api& other) const;
        object operator+= (const object_api& other);
        object operator- (const object_api& other) const;
        object operator-= (const object_api& other);
        object operator* (const object_api& other) const;
        object operator*= (const object_api& other);
        object operator/ (const object_api& other) const;
        object operator/= (const object_api& other);
        object operator| (const object_api& other) const;
        object operator|= (const object_api& other);
        object operator& (const object_api& other) const;
        object operator&= (const object_api& other);
        object operator^ (const object_api& other) const;
        object operator^= (const object_api& other);
        object operator<< (const object_api& other) const;
        object operator<<= (const object_api& other);
        object operator>> (const object_api& other) const;
        object operator>>= (const object_api& other);

        pybind11::str str() const;

        str_attr_accessor doc() const;

        int ref_count() const;
    };

    template <typename T>
    constexpr inline bool is_pyobject_v = std::is_base_of_v<pyobject_tag, T>;

    class handle : public object_api<handle>
    {
    protected:
        pkpy::PyObject* m_ptr = nullptr;
        int* ref_count = nullptr;

    public:
        handle() = default;

        handle(pkpy::PyObject* ptr) : m_ptr(ptr) {}

    public:
        auto ptr() const { return m_ptr; }

        auto& ptr() { return m_ptr; }

        explicit operator bool () const { return m_ptr != nullptr; }

        template <typename T>
        T cast() const;

        const handle& inc_ref()
        {
            PK_DEBUG_ASSERT(m_ptr != nullptr);
            if(ref_count == nullptr)
            {
                auto iter = _ref_counts_map->find(m_ptr);
                if(iter == _ref_counts_map->end())
                {
                    ref_count = ::new int(1);
                    _ref_counts_map->insert({m_ptr, ref_count});
                }
                else
                {
                    ref_count = iter->second;
                    *ref_count += 1;
                }
            }
            else
            {
                *ref_count += 1;
            }
            return *this;
        }

        const handle& dec_ref()
        {
            PK_DEBUG_ASSERT(m_ptr != nullptr);

            if(ref_count == nullptr)
            {
                auto iter = _ref_counts_map->find(m_ptr);
                if(iter == _ref_counts_map->end())
                {
                    return *this;
                }
                ref_count = iter->second;
            }

            *ref_count -= 1;

            if(*ref_count == 0)
            {
                _ref_counts_map->erase(m_ptr);
                ::delete ref_count;
                ref_count = nullptr;
            }
            return *this;
        }
    };

    class object : public handle
    {
    public:
        object() = default;

        object(const object& other) : handle(other) { inc_ref(); }

        object(object&& other) noexcept : handle(other)
        {
            other.m_ptr = nullptr;
            other.ref_count = nullptr;
        }

        ~object() { dec_ref(); }

        // Calling cast() on an object lvalue just copies (via handle::cast)
        template <typename T>
        T cast() const&;
        // Calling on an object rvalue does a move, if needed and/or possible
        template <typename T>
        T cast() &&;

    protected:
        struct borrowed_t
        {
        };

        struct stolen_t
        {
        };

        object(handle h, borrowed_t) : handle(h) { inc_ref(); }

        object(handle h, stolen_t) : handle(h) {}

        template <typename T>
        friend T reinterpret_borrow(handle);

        template <typename T>
        friend T reinterpret_steal(handle);
    };

    template <typename T>
    T reinterpret_borrow(handle h)
    {
        return {h, object::borrowed_t{}};
    }

    template <typename T>
    T reinterpret_steal(handle h)
    {
        return {h, object::stolen_t{}};
    }

    template <typename Derived>
    object object_api<Derived>::operator- () const
    {
        return vm->call_method(derived().ptr(), pkpy::__neg__);
    }

    template <typename Derived>
    object object_api<Derived>::operator~() const
    {
        return vm->call_method(derived().ptr(), pkpy::__invert__);
    }

    template <typename Derived>
    object object_api<Derived>::operator+ (const object_api& other) const
    {
        return vm->call_method(derived().ptr(), pkpy::__add__, other.derived().ptr());
    }

    template <typename Derived>
    object object_api<Derived>::operator+= (const object_api& other)
    {
        
    }

    template <typename Policy>
    class accessor : public object_api<accessor<Policy>>
    {
        using key_type = typename Policy::key_type;

    public:
        accessor(handle obj, key_type key) : obj(obj), key(std::move(key)) {}

        accessor(const accessor&) = default;
        accessor(accessor&&) noexcept = default;

        // accessor overload required to override default assignment operator (templates are not
        // allowed to replace default compiler-generated assignments).
        void operator= (const accessor& a) && { std::move(*this).operator= (handle(a)); }

        void operator= (const accessor& a) & { operator= (handle(a)); }

        template <typename T>
        void operator= (T&& value) &&
        {
            Policy::set(obj, key, object_or_cast(std::forward<T>(value)));
        }

        template <typename T>
        void operator= (T&& value) &
        {
            get_cache() = ensure_object(object_or_cast(std::forward<T>(value)));
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

    struct obj_attr
    {
        using key_type = object;

        static object get(handle obj, handle key) { return getattr(obj, key); }

        static void set(handle obj, handle key, handle val) { setattr(obj, key, val); }
    };

    struct str_attr
    {
        using key_type = const char*;

        static object get(handle obj, const char* key) { return getattr(obj, key); }

        static void set(handle obj, const char* key, handle val) { setattr(obj, key, val); }
    };

    struct generic_item
    {
        using key_type = object;

        static object get(handle obj, handle key)
        {
            pkpy::PyObject* result = vm->call_method(obj.ptr(), pkpy::__getitem__, key.ptr());
            return reinterpret_borrow<object>(result);
        }

        static void set(handle obj, handle key, handle val) { vm->call_method(obj.ptr(), pkpy::__setitem__, key.ptr(), val.ptr()); }
    };

};  // namespace pybind11

