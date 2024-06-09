#include "builtins.h"

namespace pybind11 {

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

    accessor(const handle& obj, key_type key) : m_obj(obj), m_value(), m_key(key) {}

public:
    pkpy::PyVar ptr() const {
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

    template <typename T>
    T cast() const {
        return operator handle ().template cast<T>();
    }

    operator handle () const { return ptr(); }
};

namespace policy {
struct attr {
    using key_type = pkpy::StrName;

    static pkpy::PyVar get(const handle& obj, pkpy::StrName key) { return vm->getattr(obj.ptr(), key); }

    template <typename Value>
    static void set(const handle& obj, pkpy::StrName key, Value&& value) {
        vm->setattr(obj.ptr(), key, value.ptr());
    }
};

struct item {
    using key_type = handle;

    static pkpy::PyVar get(const handle& obj, const handle& key) {
        return vm->call(vm->py_op("getitem"), obj.ptr(), key.ptr());
    }

    template <typename Value>
    static void set(const handle& obj, const handle& key, Value&& value) {
        vm->call(vm->py_op("setitem"), obj.ptr(), key.ptr(), value.ptr());
    }
};

struct tuple {
    using key_type = int;

    static pkpy::PyVar get(const handle& obj, int key) { return obj._as<pkpy::Tuple>()[key]; }

    template <typename Value>
    static void set(const handle& obj, size_t key, Value&& value) {
        obj._as<pkpy::Tuple>()[key] = std::forward<Value>(value).ptr();
    }
};

struct list {
    using key_type = int;

    static pkpy::PyVar get(const handle& obj, size_t key) { return obj._as<pkpy::List>()[key]; }

    template <typename Value>
    static void set(const handle& obj, size_t key, Value&& value) {
        obj._as<pkpy::List>()[key] = std::forward<Value>(value).ptr();
    }
};
}  // namespace policy

// implement other methods of interface

template <typename Derived>
inline attr_accessor interface<Derived>::attr(const char* key) const {
    return attr_accessor(this->ptr(), pkpy::StrName(key));
}

template <typename Derived>
inline attr_accessor interface<Derived>::attr(const handle& key) const {
    return attr_accessor(this->ptr(), pkpy::StrName(key._as<pkpy::Str>()));
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (int index) const {
    return item_accessor(this->ptr(), int_(index));
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (const char* key) const {
    return item_accessor(this->ptr(), str(key));
}

template <typename Derived>
inline item_accessor interface<Derived>::operator[] (const handle& key) const {
    return item_accessor(this->ptr(), key);
}

template <typename Derived>
template <return_value_policy policy, typename... Args>
object interface<Derived>::operator() (Args&&... args) const {
    // TODO: implement, resolve named arguments

    // vm->vectorcall()
}

inline tuple_accessor tuple::operator[] (int i) const { return tuple_accessor(this->ptr(), i); }

inline list_accessor list::operator[] (int i) const { return list_accessor(this->ptr(), i); }

}  // namespace pybind11
