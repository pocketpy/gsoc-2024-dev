#pragma once

#include "instance.h"

#define PYBIND11_LOGICAL_OPERATOR(op, name)                                    \
    bool operator op (const object_api& other) const                           \
    {                                                                          \
        return vm->py_##name(derived().ptr(), other.derived().ptr());          \
    }

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

    template <typename T>
    T& _builtin_cast(handle obj);

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
        const Derived& derived() const
        {
            return static_cast<const Derived&>(*this);
        }

    public:
        PYBIND11_LOGICAL_OPERATOR(==, eq);
        PYBIND11_LOGICAL_OPERATOR(!=, ne);
        PYBIND11_LOGICAL_OPERATOR(<, lt);
        PYBIND11_LOGICAL_OPERATOR(<=, le);
        PYBIND11_LOGICAL_OPERATOR(>, gt);
        PYBIND11_LOGICAL_OPERATOR(>=, ge);

        object operator- () const;
        object operator~() const;

        object operator+ (const object_api& other) const;
        object operator- (const object_api& other) const;
        object operator* (const object_api& other) const;
        object operator% (const object_api& other) const;
        object operator/ (const object_api& other) const;
        object operator| (const object_api& other) const;
        object operator& (const object_api& other) const;
        object operator^ (const object_api& other) const;
        object operator<< (const object_api& other) const;
        object operator>> (const object_api& other) const;

        object operator+= (const object_api& other);
        object operator-= (const object_api& other);
        object operator*= (const object_api& other);
        object operator/= (const object_api& other);
        object operator%= (const object_api& other);
        object operator|= (const object_api& other);
        object operator&= (const object_api& other);
        object operator^= (const object_api& other);
        object operator<<= (const object_api& other);
        object operator>>= (const object_api& other);

        template <return_value_policy policy =
                      return_value_policy::automatic_reference,
                  typename... Args>
        object operator() (Args&&... args) const;

        args_proxy operator* () const;

        item_accessor operator[] (handle key) const;

        item_accessor operator[] (object&& key) const;

        item_accessor operator[] (const char* key) const;

        obj_attr_accessor attr(handle key) const;

        obj_attr_accessor attr(object&& key) const;

        str_attr_accessor attr(const char* key) const;

        iterator begin() const;

        iterator end() const;

        template <typename T>
        bool contains(T&& item) const;

        /// Equivalent to ``obj is None`` in Python.
        bool is_none() const { return derived().ptr() == vm->None; }

        /// Equivalent to ``obj is other`` in Python.
        bool is(const object_api& other) const
        {
            return derived().ptr() == other.derived().ptr();
        }

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

        object& operator= (object&& other) noexcept
        {
            if(this != &other)
            {
                dec_ref();
                m_ptr = other.m_ptr;
                ref_count = other.ref_count;
                other.m_ptr = nullptr;
                other.ref_count = nullptr;
            }
            return *this;
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

};  // namespace pybind11

#undef PYBIND11_LOGICAL_OPERATOR
