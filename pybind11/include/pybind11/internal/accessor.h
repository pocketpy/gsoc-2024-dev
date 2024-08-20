#pragma once

#include "builtins.h"

namespace pkbind {

template <typename policy>
class accessor : public interface<accessor<policy>> {

    using key_type = typename policy::key_type;

    handle m_obj;
    mutable handle m_value;
    key_type m_key;

    friend interface<handle>;
    friend interface<accessor<policy>>;
    friend tuple;
    friend list;
    friend dict;

    accessor(handle obj, key_type key) : m_obj(obj), m_value(), m_key(key) {}

public:
    auto ptr() const {
        if(!m_value) { m_value = policy::get(m_obj, m_key); }
        return m_value.ptr();
    }

    template <typename Value>
    accessor& operator= (Value&& value) && {
        policy::set(m_obj, m_key, std::forward<Value>(value));
        return *this;
    }

    template <typename Value>
    accessor& operator= (Value&& value) & {
        m_value = std::forward<Value>(value);
        return *this;
    }

    operator handle () const { return ptr(); }
};

namespace policy {
struct attr {
    using key_type = name;

    static handle get(handle obj, name key) {
        raise_call<py_getattr>(obj.ptr(), key.index());
        return py_retval();
    }

    static void set(handle obj, name key, handle value) { raise_call<py_setattr>(obj.ptr(), key.index(), value.ptr()); }

    template <typename Value>
    static void set(handle obj, name key, Value&& value) {
        raise_call<py_setattr>(obj.ptr(), key.index(), pkbind::cast(std::forward<Value>(value)).ptr());
    }
};

struct item {
    using key_type = handle;

    static handle get(handle obj, handle key) {
        raise_call<py_getitem>(obj.ptr(), key.ptr());
        return py_retval();
    }

    static void set(handle obj, handle key, handle value) { raise_call<py_setitem>(obj.ptr(), key.ptr(), value.ptr()); }

    template <typename Value>
    static void set(handle obj, handle key, Value&& value) {
        raise_call<py_setitem>(obj.ptr(), key.ptr(), pkbind::cast(std::forward<Value>(value)).ptr());
    }
};

struct tuple {
    using key_type = int;

    static handle get(handle obj, int key) { return py_tuple_getitem(obj.ptr(), key); }

    static void set(handle obj, int key, handle value) { py_tuple_setitem(obj.ptr(), key, value.ptr()); }
};

struct list {
    using key_type = int;

    static handle get(handle obj, int key) { return py_list_getitem(obj.ptr(), key); }

    static void set(handle obj, int key, handle value) { py_list_setitem(obj.ptr(), key, value.ptr()); }
};

struct dict {
    using key_type = handle;

    static handle get(handle obj, handle key) {
        raise_call<py_dict_getitem>(obj.ptr(), key.ptr());
        return py_retval();
    }

    static void set(handle obj, handle key, handle value) {
        raise_call<py_dict_setitem>(obj.ptr(), key.ptr(), value.ptr());
    }
};

}  // namespace policy

// implement other methods of interface

template <typename Derived>
inline attr_accessor interface<Derived>::attr(name key) const {
    return attr_accessor(ptr(), key);
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (int key) const {
    return item_accessor(ptr(), int_(key));
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (name key) const {
    return item_accessor(ptr(), str(key));
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (handle key) const {
    return item_accessor(ptr(), key);
}

inline tuple_accessor tuple::operator[] (int i) const { return tuple_accessor(m_ptr, i); }

inline list_accessor list::operator[] (int i) const { return list_accessor(m_ptr, i); }

inline dict_accessor dict::operator[] (int index) const { return dict_accessor(m_ptr, int_(index)); }

inline dict_accessor dict::operator[] (name key) const {
    return dict_accessor(m_ptr, pkbind::str(std::string_view(key)));
}

inline dict_accessor dict::operator[] (handle key) const { return dict_accessor(this->ptr(), key); }

}  // namespace pkbind
