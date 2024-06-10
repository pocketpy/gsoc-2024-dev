#pragma once
#include "object.h"

namespace pybind11 {

struct type_visitor {
    template <typename T>
    constexpr static auto type_or_check() {
        return T::type_or_check;
    }
};

template <typename T, typename = void>
constexpr inline static bool has_underlying_type = false;

template <typename T>
constexpr inline static bool has_underlying_type<T, std::void_t<typename T::type_or_check>> = true;

#define PYBIND11_TYPE_IMPLEMENT(parent, name, tp)                                                                      \
                                                                                                                       \
private:                                                                                                               \
    using underlying_type = name;                                                                                      \
    constexpr inline static auto type_or_check = tp;                                                                   \
    decltype(auto) value() const { return _as<underlying_type>(); }                                                    \
    template <typename... Args>                                                                                        \
    static handle create(Args&&... args) {                                                                             \
        return vm->new_object<underlying_type>(type_or_check, std::forward<Args>(args)...);                            \
    }                                                                                                                  \
    friend type_visitor;                                                                                               \
    using parent::parent;

class none : public object {
    PYBIND11_TYPE_IMPLEMENT(object, empty, vm->tp_none_type);

public:
    none() : object(vm->None) {}
};

/// corresponding to type in Python
class type : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Type, vm->tp_type);

public:
    template <typename T>
    static handle handle_of() {
        if constexpr(has_underlying_type<T>) {
            return T::underlying_type;
        } else {
            auto type = vm->_cxx_typeid_map.try_get(typeid(T));
            if(type) {
                return vm->_t(*type);
            } else {
                vm->TypeError("type not registered");
            }
        }
    }
};

/// corresponding to bool in Python
class bool_ : public object {
    PYBIND11_TYPE_IMPLEMENT(object, bool, vm->tp_bool);

public:
    bool_(bool value) : object(create(value)) {}

    operator bool () const { return value(); }
};

/// corresponding to int in Python
class int_ : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::i64, vm->tp_int);

public:
    int_(int64_t value) : object(create(value)) {}

    operator int64_t () const { return value(); }
};

/// corresponding to float in Python
class float_ : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::f64, vm->tp_float);

public:
    float_(double value) : object(create(value)) {}

    operator double () const { return value(); }
};

class iterable : public object {
    PYBIND11_TYPE_IMPLEMENT(object, empty, [](const handle& obj) {
        return vm->getattr(obj.ptr(), pkpy::__iter__, false) != nullptr;
    });
};

class iterator : public object {
    PYBIND11_TYPE_IMPLEMENT(object, empty, [](const handle& obj) {
        return vm->getattr(obj.ptr(), pkpy::__next__, false) != nullptr &&
               vm->getattr(obj.ptr(), pkpy::__iter__, false) != nullptr;
    });

    handle m_value;

public:
    iterator(const handle& obj) : object(obj) { m_value = vm->py_next(obj.ptr()); }

    iterator operator++ () {
        m_value = vm->py_next(m_ptr);
        return *this;
    }

    iterator operator++ (int) {
        m_value = vm->py_next(m_ptr);
        return *this;
    }

    const handle& operator* () const { return m_value; }

    friend bool operator== (const iterator& lhs, const iterator& rhs) { return lhs.m_value.is(rhs.m_value); }

    friend bool operator!= (const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }

    static iterator sentinel() {
        iterator iter;
        iter.m_ptr = vm->None;
        iter.m_value = vm->StopIteration;
        return iter;
    }
};

class str : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Str, vm->tp_str);

public:
    str(const char* c, int len) : object(create(c, len)) {};

    str(const char* c = "") : str(c, strlen(c)) {}

    str(const std::string& s) : str(s.data(), s.size()) {}

    str(std::string_view sv) : str(sv.data(), sv.size()) {}

    // explicit str(const bytes& b);
    explicit str(handle h);
    operator std::string () const;

    template <typename... Args>
    str format(Args&&... args) const;
};

// class bytes : public object {
// public:
//     using object::object;
// };

// class bytearray : public object {
// public:
//     using object::object;
// };

class tuple : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Tuple, vm->tp_tuple);

public:
    tuple(int n) : object(create(n)) {}

    template <typename... Args, std::enable_if_t<(sizeof...(Args) > 0)>* = nullptr>
    tuple(Args&&... args) : object(create(sizeof...(Args))) {
        int index = 0;
        ((value()[index++] = pybind11::cast(std::forward<Args>(args)).ptr()), ...);
    }

    int size() const { return value().size(); }

    bool empty() const { return size() == 0; }

    tuple_accessor operator[] (int i) const;
};

class list : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::List, vm->tp_list)

public:
    list() : object(create()) {}

    list(int n) : object(create(n)) {}

    template <typename... Args>
    list(Args&&... args) : object(create(sizeof...(Args))) {
        int index = 0;
        ((value()[index++] = pybind11::cast(std::forward<Args>(args)).ptr()), ...);
    }

    int size() const { return value().size(); }

    bool empty() const { return size() == 0; }

    void clear() { value().clear(); }

    list_accessor operator[] (int i) const;

    void append(const handle& obj) { value().push_back(obj.ptr()); }

    void extend(const handle& iterable) {
        for(auto& item: iterable) {
            append(item);
        }
    }

    void insert(int index, const handle& obj) {
        const auto pos = value().begin() + index;
        value().insert(pos, obj.ptr());
    }
};

// class set : public object {
// public:
//     using object::object;
//     // set() : object(vm->new_object<pkpy::Se>(pkpy::VM::tp_set), true) {}
// };
//

class dict : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Dict, vm->tp_dict);

public:
    dict() : object(create()) {}

    int size() const { return value().size(); }

    bool empty() const { return size() == 0; }

    void clear() { value().clear(); }

    dict_accessor operator[] (int index) const;
    dict_accessor operator[] (std::string_view) const;
    dict_accessor operator[] (const handle& key) const;
};

class function : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Function, vm->tp_function);
};

//
// class buffer : public object {
// public:
//    using object::object;
//};
//
// class memory_view : public object {
// public:
//    using object::object;
//};
//
// class capsule : public object {
// public:
//    using object::object;
//};
//

class args : public tuple {
    PYBIND11_TYPE_IMPLEMENT(tuple, struct empty, vm->tp_tuple);
};

class kwargs : public dict {
    PYBIND11_TYPE_IMPLEMENT(dict, struct empty, vm->tp_dict);
};

#undef PYBIND11_TYPE_IMPLEMENT

// implement iterator methods for interface
template <typename Derived>
inline iterator interface<Derived>::begin() const {
    return handle(vm->py_iter(this->ptr()));
}

template <typename Derived>
inline iterator interface<Derived>::end() const {
    return iterator::sentinel();
}

}  // namespace pybind11
