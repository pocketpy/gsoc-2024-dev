#pragma once
#include "kernel.h"

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

/// pkpy does not use reference counts, so object is just fot API compatibility
class object : public handle {
public:
    using handle::handle;

    object(const handle& h) : handle(h) {}
};

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
#undef PYBIND11_BINARY_LOGIC_OPERATOR

template <typename T>
constexpr inline bool is_pyobject_v = std::is_base_of_v<object, T>;

template <typename T>
handle cast(T&& value, return_value_policy policy = return_value_policy::automatic_reference, handle parent = {});
};  // namespace pybind11
