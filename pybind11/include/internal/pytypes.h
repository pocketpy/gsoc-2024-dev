#pragma once

#include "object.h"

namespace pybind11
{
    class type : public object
    {
    public:
        using object::object;

        static handle handle_of(handle h) { return vm->_t(h.ptr()); }

        static type of(handle h) { return reinterpret_borrow<type>(handle_of(h)); }

        template <typename T>
        static handle handle_of()
        {
            auto result = vm->_cxx_typeid_map.find(typeid(T));
            if(result != vm->_cxx_typeid_map.end())
            {
                return vm->_t(result->second);
            }

            // TODO: clarify the error message
            throw std::runtime_error("Type not found");
        }

        template <typename T>
        static type of()
        {
            return reinterpret_borrow<type>(handle_of<T>());
        }
    };

    class iterator : public object
    {

    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = uint64_t;
        using value_type = handle;
        using reference = const handle;  // PR #3263
        using pointer = const handle*;

    public:
        iterator& operator++ ();

        iterator operator++ (int);

        reference operator* () const;

        pointer operator->() const;

        static iterator sentinel() { return {}; }

        friend bool operator== (const iterator& a, const iterator& b) { return a->ptr() == b->ptr(); }

        friend bool operator!= (const iterator& a, const iterator& b) { return a->ptr() != b->ptr(); }
    };

    class iterable : public object
    {
    };

    class str : public object
    {
    public:
        str(const char* c, int len) { m_ptr = vm->heap.gcnew<pkpy::Str>(pkpy::VM::tp_str, c, len); }

        str(const char* c = "") : str(c, strlen(c)) {}

        str(const std::string& s) : str(s.data(), s.size()) {}

        str(std::string_view sv) : str(sv.data(), sv.size()) {}

        explicit str(const bytes& b);
        explicit str(handle h);
        operator std::string () const;

        template <typename... Args>
        str format(Args&&... args) const;
    };

    class bytes : public object
    {
    public:
        bytes(const char* c = "");
        bytes(const char* c, int n);
        bytes(const std::string& s);
        bytes(std::string_view sv);
        explicit bytes(const pybind11::str& s);
        explicit bytes(handle h);
        operator std::string () const;
    };

    class bytearray : public object
    {
    public:
    };

    class none : public object
    {
    };

    class ellipsis : public object
    {
    };

    class bool_ : public object
    {
    };

    class int_ : public object
    {
    };

    class float_ : public object
    {
    };

    class slice : public object
    {
    };

    class capsule : public object
    {
    };

    class tuple : public object
    {
    };

    class dict : public object
    {
    };

    class sequence : public object
    {
    };

    class list : public object
    {
    };

    class args : public tuple
    {
    };

    class kwargs : public dict
    {
    };

    class anyset : public object
    {
    };

    class set : public anyset
    {
    };

    class frozenset : public anyset
    {
    };

    class function : public object
    {
    };

    class staticmethod : public object
    {
    };

    class buffer : public object
    {
    };

    class memoryview : public object
    {
    };
}  // namespace pybind11
