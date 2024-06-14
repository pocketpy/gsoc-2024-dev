#pragma once
#include "object.h"

namespace pybind11 {

template <typename T>
handle cast(T&& value, return_value_policy policy = return_value_policy::automatic_reference, handle parent = {});

// undef in pybind11.h
#define PYBIND11_REGISTER_INIT(func)                                                                                   \
    static inline int _register = [] {                                                                                 \
        if(_init == nullptr) _init = new std::vector<void (*)()>();                                                    \
        _init->push_back(func);                                                                                        \
        return 0;                                                                                                      \
    }();

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
        return type_visitor::type<T>();
    }

    static type of(const handle& obj) { return type(vm->_t(obj.ptr())); }
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

    operator std::string_view () const { return value().sv(); }

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

    template <typename... Args, std::enable_if_t<(sizeof...(Args) > 1)>* = nullptr>
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

    template <typename... Args, std::enable_if_t<(sizeof...(Args) > 1)>* = nullptr>
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
class capsule : public object {
    struct pack {
        void* data;
        void (*destructor)(void*);

        template <typename T>
        pack(T&& value) {
            data = new auto(std::forward<T>(value));
            destructor = [](void* ptr) {
                delete static_cast<std::decay_t<T>*>(ptr);
            };
        }

        ~pack() { destructor(data); }
    };

    PYBIND11_REGISTER_INIT([] {
        handle type = vm->new_type_object<pack>(vm->builtins, "capsule", vm->tp_object, false);
        vm->builtins->attr().set("capsule", type.ptr());
    });

    PYBIND11_TYPE_IMPLEMENT(object, pack, handle(vm->builtins->attr("capsule"))._as<pkpy::Type>());

public:
    template <typename T>
    capsule(T&& value) : object(create(std::forward<T>(value))) {}

    template <typename T>
    T& cast() const {
        return *static_cast<T*>(value().data);
    }
};

class property : public object {
    PYBIND11_TYPE_IMPLEMENT(object, pkpy::Property, vm->tp_property);

public:
    property(handle getter, handle setter) : object(create(getter.ptr(), setter.ptr())) {}

    handle getter() const { return value().getter; }

    handle setter() const { return value().setter; }
};

class args : public tuple {
    PYBIND11_TYPE_IMPLEMENT(tuple, struct empty, vm->tp_tuple);
};

class kwargs : public dict {
    PYBIND11_TYPE_IMPLEMENT(dict, struct empty, vm->tp_dict);
};
}  // namespace pybind11
