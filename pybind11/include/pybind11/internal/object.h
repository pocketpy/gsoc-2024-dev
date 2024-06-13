#pragma once
#include "instance.h"

namespace pybind11 {

template <typename Derived>
class interface {
private:
    pkpy::PyVar ptr() const { return static_cast<const Derived*>(this)->ptr(); }

public:
    bool is_none() const { return ptr() == vm->None; }

    bool is(const interface& other) const { return ptr() == other.ptr(); }

    bool in(const interface& other) const {
        return pybind11::cast<bool>(vm->call(vm->py_op("contains"), other.ptr(), ptr()));
    }

    bool contains(const interface& other) const {
        return pybind11::cast<bool>(vm->call(vm->py_op("contains"), ptr(), other.ptr()));
    }

    iterator begin() const;
    iterator end() const;

    attr_accessor attr(const char* key) const;
    attr_accessor attr(const handle& key) const;

    item_accessor operator[] (int index) const;
    item_accessor operator[] (const char* key) const;
    item_accessor operator[] (const handle& key) const;

    template <return_value_policy policy = return_value_policy::automatic, typename... Args>
    object operator() (Args&&... args) const;

    object operator- () const;
    object operator~() const;
};

/// a lightweight wrapper for python objects
class handle : public interface<handle> {
protected:
    mutable pkpy::PyVar m_ptr = nullptr;

public:
    handle() = default;
    handle(const handle& h) = default;
    handle& operator= (const handle& other) = default;

    handle(std::nullptr_t) = delete;

    handle(pkpy::PyVar ptr) : m_ptr(ptr) {}

    handle(pkpy::PyObject* ptr) : m_ptr(ptr) {}

    pkpy::PyVar ptr() const { return m_ptr; }

    explicit operator bool () const { return m_ptr != nullptr; }

public:
    template <typename T>
    T cast() const;

    // this is a internal function, use to interact with pocketpy python
    template <typename T>
    decltype(auto) _as() const {
        static_assert(!std::is_reference_v<T>, "T must not be a reference type.");
        return m_ptr.obj_get<T>();
    }
};

// a helper class to visit type
struct type_visitor {

    template <typename T>
    constexpr static bool is_type = std::is_same_v<pkpy::Type, std::decay_t<decltype(T::type_or_check())>>;

    template <typename T>
    static pkpy::Type type() {
        if constexpr(is_pyobject_v<T>) {
            if constexpr(is_type<T>) {
                // for some type, they have according type in python, e.g. bool, int, float
                // so just return the according type
                return T::type_or_check();
            } else {
                // for other type, they don't have according type in python, like iterable, iterator
                static_assert(dependent_false<T>, "type_or_check not defined");
            }
        } else {
            // for C++ type, lookup the type in the type map
            auto type = vm->_cxx_typeid_map.try_get(typeid(T));
            // if found, return the type
            if(type) return *type;
            // if not found, raise error
            vm->TypeError("type not registered");
        }
    }

    template <typename T>
    static bool check(const handle& obj) {
        if constexpr(is_pyobject_v<T>) {
            if constexpr(is_type<T>) {
                return vm->isinstance(obj.ptr(), T::type_or_check());
            } else {
                // some type, like iterable, iterator, they don't have according type in python
                // but they have a function to check the type, then just call the function
                return T::type_or_check(obj);
            }
        } else {
            return vm->isinstance(obj.ptr(), type<T>());
        }
    }
};

#define PYBIND11_TYPE_IMPLEMENT(parent, name, tp)                                                                      \
                                                                                                                       \
private:                                                                                                               \
    using underlying_type = name;                                                                                      \
    inline static auto type_or_check = [] {                                                                            \
        return tp;                                                                                                     \
    };                                                                                                                 \
    decltype(auto) value() const { return _as<underlying_type>(); }                                                    \
    template <typename... Args>                                                                                        \
    static handle create(Args&&... args) {                                                                             \
        return vm->new_object<underlying_type>(type_or_check(), std::forward<Args>(args)...);                          \
    }                                                                                                                  \
    friend type_visitor;                                                                                               \
    using parent::parent;

/*=============================================================================//
// pkpy does not use reference counts, so object is just fot API compatibility //
//=============================================================================*/

class object : public handle {
    PYBIND11_TYPE_IMPLEMENT(handle, pkpy::PyObject, vm->tp_object);

public:
    object(const handle& h) : handle(h) {}
};

// implement some global functions
#define PYBIND11_BINARY_OPERATOR(OP, NAME)                                                                             \
    inline object operator OP (const handle& lhs, const handle& rhs) {                                                 \
        return vm->call(vm->py_op(NAME), lhs.ptr(), rhs.ptr());                                                        \
    }

#define PYBIND11_INPLACE_OPERATOR(OP, NAME)                                                                            \
    inline object operator OP (handle& lhs, const handle& rhs) {                                                       \
        handle result = vm->call(vm->py_op(NAME), lhs.ptr(), rhs.ptr());                                               \
        return lhs = result;                                                                                           \
    }

#define PYBIND11_BINARY_LOGIC_OPERATOR(OP, NAME)                                                                       \
    inline bool operator OP (const handle& lhs, const handle& rhs) {                                                   \
        return pybind11::cast<bool>(vm->call(vm->py_op(NAME), lhs.ptr(), rhs.ptr()));                                  \
    }

PYBIND11_BINARY_OPERATOR(+, "add");
PYBIND11_BINARY_OPERATOR(-, "sub");
PYBIND11_BINARY_OPERATOR(*, "mul");
PYBIND11_BINARY_OPERATOR(/, "truediv");
PYBIND11_BINARY_OPERATOR(%, "mod");
PYBIND11_BINARY_OPERATOR(|, "or_");
PYBIND11_BINARY_OPERATOR(&, "and_");
PYBIND11_BINARY_OPERATOR(^, "xor");
PYBIND11_BINARY_OPERATOR(<<, "lshift");
PYBIND11_BINARY_OPERATOR(>>, "rshift");

PYBIND11_INPLACE_OPERATOR(+=, "iadd");
PYBIND11_INPLACE_OPERATOR(-=, "isub");
PYBIND11_INPLACE_OPERATOR(*=, "imul");
PYBIND11_INPLACE_OPERATOR(/=, "itruediv");
PYBIND11_INPLACE_OPERATOR(%=, "imod");
PYBIND11_INPLACE_OPERATOR(|=, "ior");
PYBIND11_INPLACE_OPERATOR(&=, "iand");
PYBIND11_INPLACE_OPERATOR(^=, "ixor");
PYBIND11_INPLACE_OPERATOR(<<=, "ilshift");
PYBIND11_INPLACE_OPERATOR(>>=, "irshift");

PYBIND11_BINARY_LOGIC_OPERATOR(==, "eq");
PYBIND11_BINARY_LOGIC_OPERATOR(!=, "ne");
PYBIND11_BINARY_LOGIC_OPERATOR(<, "lt");
PYBIND11_BINARY_LOGIC_OPERATOR(>, "gt");
PYBIND11_BINARY_LOGIC_OPERATOR(<=, "le");
PYBIND11_BINARY_LOGIC_OPERATOR(>=, "ge");

#undef PYBIND11_BINARY_OPERATOR
#undef PYBIND11_INPLACE_OPERATOR
#undef PYBIND11_BINARY_LOGIC_OPERATOR
};  // namespace pybind11
