#pragma once

#include "object.h"

namespace pybind11
{
    class type : public object
    {
    public:
        using object::object;

        static handle handle_of(handle h) { return vm->_t(h.ptr()); }

        static type of(handle h)
        {
            return reinterpret_borrow<type>(handle_of(h));
        }

        template <typename T>
        static handle handle_of()
        {
            if constexpr(std::is_same_v<T, type>)
            {
                return vm->_t(vm->tp_type);
            }
            else if constexpr(std::is_same_v<T, class iterator>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class iterable>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class str>)
            {
                return vm->_t(vm->tp_str);
            }
            else if constexpr(std::is_same_v<T, class bytes>)
            {
                return vm->_t(vm->tp_bytes);
            }
            else if constexpr(std::is_same_v<T, class bytearray>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class none>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class ellipsis>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class bool_>)
            {
                return vm->_t(vm->tp_bool);
            }
            else if constexpr(std::is_same_v<T, class int_>)
            {
                return vm->_t(vm->tp_int);
            }
            else if constexpr(std::is_same_v<T, class float_>)
            {
                return vm->_t(vm->tp_float);
            }
            else if constexpr(std::is_same_v<T, class slice>)
            {
                return vm->_t(vm->tp_slice);
            }
            else if constexpr(std::is_same_v<T, class capsule>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class tuple>)
            {
                return vm->_t(vm->tp_tuple);
            }
            else if constexpr(std::is_same_v<T, class dict>)
            {
                return vm->_t(vm->tp_dict);
            }
            else if constexpr(std::is_same_v<T, class sequence>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class list>)
            {
                return vm->_t(vm->tp_list);
            }
            else if constexpr(std::is_same_v<T, class args>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class kwargs>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class anyset>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class set>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class frozenset>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class function>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class staticmethod>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class buffer>)
            {
                // FIXME:
                return nullptr;
            }
            else if constexpr(std::is_same_v<T, class memoryview>)
            {
                // FIXME:
                return nullptr;
            }
            else
            {
                auto result = vm->_cxx_typeid_map.find(typeid(T));
                if(result != vm->_cxx_typeid_map.end())
                {
                    return vm->_t(result->second);
                }

                // TODO: clarify the error message
                throw std::runtime_error("Type not found");
            }
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

        friend bool operator== (const iterator& a, const iterator& b)
        {
            return a->ptr() == b->ptr();
        }

        friend bool operator!= (const iterator& a, const iterator& b)
        {
            return a->ptr() != b->ptr();
        }
    };

    class iterable : public object
    {
    };

    class str : public object
    {
    public:
        str(const char* c, int len)
        {
            m_ptr = vm->heap.gcnew<pkpy::Str>(pkpy::VM::tp_str, c, len);
            inc_ref();
        }

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

    class args_proxy : object
    {
    };
}  // namespace pybind11
