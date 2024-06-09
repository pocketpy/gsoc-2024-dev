#pragma once

#include "object.h"

namespace pybind11 {
    class none : public object {
    public:
        using object::object;

        none() : object(vm->None, false) {}
    };

    class type : public object {
    public:
        using object::object;
        template <typename T>
        static handle handle_of();
    };

    class bool_ : public object {
    public:
        using object::object;

        bool_(bool value) : object(pkpy::py_var(vm, value), false) {}
    };

    class int_ : public object {
    public:
        using object::object;

        int_(int64_t value) : object(pkpy::py_var(vm, value), true) {}
    };

    class float_ : public object {
    public:
        using object::object;

        float_(double value) : object(pkpy::py_var(vm, value), true) {}
    };

    class iterable : public object {
    public:
        using object::object;
        iterable() = delete;
    };

    class iterator : public object {
        handle value;

        iterator(pkpy::PyVar stop) : object(none{}, false), value(stop) {}

    public:
        using object::object;
        iterator() = delete;

        iterator(object&& obj) : object(std::move(obj)), value(vm->py_next(m_ptr)) {}

        iterator operator++ () {
            value = vm->py_next(m_ptr);
            return *this;
        }

        iterator operator++ (int) {
            value = vm->py_next(m_ptr);
            return *this;
        }

        const handle& operator* () const { return value; }

        bool operator== (const iterator& other) const { return value.ptr() == other.value.ptr(); }

        bool operator!= (const iterator& other) const { return !(*this == other); }

        static iterator sentinel() { return iterator(vm->StopIteration); }
    };

    template <typename Derived>
    inline iterator object_api<Derived>::begin() const {
        return reinterpret_borrow<object>(vm->py_iter(this->get()));
    }

    template <typename Derived>
    inline iterator object_api<Derived>::end() const {
        return iterator::sentinel();
    }

    class str : public object {

    public:
        using object::object;
        str(const char* c, int len) :
            object(vm->new_object<pkpy::Str>(pkpy::VM::tp_str, c, len), true) {

            };

        str(const char* c = "") : str(c, strlen(c)) {}

        str(const std::string& s) : str(s.data(), s.size()) {}

        str(std::string_view sv) : str(sv.data(), sv.size()) {}

        explicit str(const bytes& b);
        explicit str(handle h);
        operator std::string () const;

        template <typename... Args>
        str format(Args&&... args) const;
    };

    class bytes : public object {
    public:
        using object::object;
    };

    class bytearray : public object {
    public:
        using object::object;
    };

    class tuple : public object {
        pkpy::Tuple& get() const { return _builtin_cast<pkpy::Tuple>(m_ptr); }

        static pkpy::PyVar create(int n) {
            return vm->new_object<pkpy::Tuple>(pkpy::VM::tp_tuple, n);
        }

    public:
        using object::object;

        explicit tuple(int n) : object(create(n), true) {}

        template <typename... Args, std::enable_if_t<(sizeof...(Args) > 0)>* = nullptr>
        tuple(Args&&... args) : object(create(sizeof...(Args)), true) {
            int index = 0;
            ((get()[index++] = _cast(std::forward<Args>(args)).ptr()), ...);
        }

        int size() const { return get().size(); }

        bool empty() const { return size() == 0; }

        tuple_accessor operator[] (int i) const;
    };

    class list : public object {
    private:
        pkpy::List& get() const { return _builtin_cast<pkpy::List>(m_ptr); }

        template <typename... Args>
        static pkpy::PyVar create(Args&&... args) {
            return vm->new_object<pkpy::List>(pkpy::VM::tp_list, std::forward<Args>(args)...);
        }

    public:
        using object::object;

        list() : object(create(), true) {}

        list(int n) : object(create(n), true) {}

        template <typename... Args>
        list(Args&&... args) : object(create(sizeof...(Args)), true) {
            int index = 0;
            ((get()[index++] = _cast(std::forward<Args>(args)).ptr()), ...);
        }

        int size() const { return get().size(); }

        bool empty() const { return size() == 0; }

        list_accessor operator[] (int i) const;

        void append(const handle& value) { get().push_back(value.ptr()); }

        void extend(const handle& iterable) {
            for(auto& item: iterable) {
                append(item);
            }
        }

        void insert(int index, const handle& value) {
            get().insert(get().begin() + index, value.ptr());
        }
    };

    class set : public object {
    public:
        using object::object;
        // set() : object(vm->new_object<pkpy::Se>(pkpy::VM::tp_set), true) {}
    };

    class dict : public object {
    public:
        using object::object;

        dict() : object(vm->new_object<pkpy::Dict>(pkpy::VM::tp_dict), true) {}
    };

    class function : public object {
    public:
        using object::object;
    };

    class buffer : public object {
    public:
        using object::object;
    };

    class memory_view : public object {
    public:
        using object::object;
    };

    class capsule : public object {
    public:
        using object::object;
    };

    class args : public tuple {
        using tuple::tuple;
    };

    class kwargs : public dict {
        using dict::dict;
    };

    template <typename T>
    handle type::handle_of() {
        if constexpr(std::is_same_v<T, object>) {
            return vm->_t(vm->tp_object);
        }
#define PYBIND11_TYPE_MAPPER(type, tp)                                                             \
    else if constexpr(std::is_same_v<T, type>) {                                                   \
        return vm->_t(vm->tp);                                                                     \
    }
        PYBIND11_TYPE_MAPPER(type, tp_type)
        PYBIND11_TYPE_MAPPER(str, tp_str)
        PYBIND11_TYPE_MAPPER(int_, tp_int)
        PYBIND11_TYPE_MAPPER(float_, tp_float)
        PYBIND11_TYPE_MAPPER(bool_, tp_bool)
        PYBIND11_TYPE_MAPPER(list, tp_list)
        PYBIND11_TYPE_MAPPER(tuple, tp_tuple)
        PYBIND11_TYPE_MAPPER(args, tp_tuple)
        PYBIND11_TYPE_MAPPER(dict, tp_dict)
        PYBIND11_TYPE_MAPPER(kwargs, tp_dict)
#undef PYBIND11_TYPE_MAPPER
        else {
            auto result = vm->_cxx_typeid_map.try_get(typeid(T));
            if(result != nullptr) {
                return vm->_t(*result);
            }

            vm->TypeError("Type not registered");
        }
    }

}  // namespace pybind11
