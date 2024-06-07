#pragma once

#include "kernel.h"

namespace pybind11 {
    class handle;
    class object;
    class iterator;
    class str;
    class bytes;
    class iterable;
    class tuple;
    class dict;
    class list;
    class set;
    class function;
    class module;
    class type;
    class bool_;
    class int_;
    class float_;
    class str;
    class bytes;

    template <typename policy>
    class accessor;

    namespace policy {
        struct attr;
        struct item;
        struct tuple;
        struct list;
    }  // namespace policy

    using attr_accessor = accessor<policy::attr>;
    using item_accessor = accessor<policy::item>;
    using tuple_accessor = accessor<policy::tuple>;
    using list_accessor = accessor<policy::list>;

    template <typename T>
    T& _builtin_cast(const handle& obj);

    template <typename T>
    T reinterpret_borrow(const handle& h);

    template <typename T>
    T reinterpret_steal(const handle& h);

    template <typename Derived>
    class object_api {
    public:
        pkpy::PyVar get() const { return static_cast<const Derived*>(this)->ptr(); }

    public:
        bool is_none() const { return get() == vm->None; }

        bool is(const object_api& other) const { return get() == other.get(); }

        bool in(const object_api& other) const {
            return pkpy::py_cast<bool>(vm, vm->call(vm->py_op("contains"), other.get(), get()));
        }

        bool contains(const object_api& other) const {
            return pkpy::py_cast<bool>(vm, vm->call(vm->py_op("contains"), get(), other.get()));
        }

        iterator begin() const;
        iterator end() const;

        str doc() const;
        attr_accessor attr(const char* key) const;
        attr_accessor attr(const object_api& key) const;

        item_accessor operator[] (int index) const;
        item_accessor operator[] (const char* key) const;
        item_accessor operator[] (const object_api& key) const;

        template <return_value_policy policy = return_value_policy::automatic, typename... Args>
        object operator() (Args&&... args) const;

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

        object operator+= (const object_api& other) const;
        object operator-= (const object_api& other) const;
        object operator*= (const object_api& other) const;
        object operator/= (const object_api& other) const;
        object operator%= (const object_api& other) const;
        object operator|= (const object_api& other) const;
        object operator&= (const object_api& other) const;
        object operator^= (const object_api& other) const;
        object operator<<= (const object_api& other) const;
        object operator>>= (const object_api& other) const;

        object operator== (const object_api& other) const;
        object operator!= (const object_api& other) const;
        object operator< (const object_api& other) const;
        object operator> (const object_api& other) const;
        object operator<= (const object_api& other) const;
        object operator>= (const object_api& other) const;
    };

    class handle : public object_api<handle> {
    protected:
        pkpy::PyVar m_ptr = nullptr;
        mutable int* ref_count = nullptr;

    public:
        handle() = default;
        handle(const handle& h) = default;
        handle& operator= (const handle& other) = default;

        handle(pkpy::PyVar ptr) : m_ptr(ptr) {}

        handle(pkpy::PyObject* ptr) : m_ptr(ptr) {}

        pkpy::PyVar ptr() const { return m_ptr; }

        int reference_count() const { return ref_count == nullptr ? 0 : *ref_count; }

        explicit operator bool () const { return m_ptr != nullptr; }

        const handle& inc_ref() const {
            assert(m_ptr != nullptr);

            if(ref_count == nullptr) {
                auto iter = _ref_counts_map->find(m_ptr);
                if(iter == _ref_counts_map->end()) {
                    ref_count = ::new int(1);
                    _ref_counts_map->insert({m_ptr, ref_count});
                } else {
                    ref_count = iter->second;
                    *ref_count += 1;
                }
            } else {
                *ref_count += 1;
            }

            return *this;
        }

        const handle& dec_ref() const {
            assert(m_ptr != nullptr);

            if(ref_count == nullptr) {
                return *this;
            }

            *ref_count -= 1;

            // if(*ref_count == 0) {
            //     _ref_counts_map->erase(m_ptr);
            //     ::delete ref_count;
            //     ref_count = nullptr;
            // }

            return *this;
        }

    public:
        template <typename T>
        T cast() const;

        template <typename T>
        friend T& _builtin_cast(const handle& obj) {
            static_assert(!std::is_reference_v<T>, "T must not be a reference type.");
            return obj.ptr().obj_get<T>();
        }
    };

    static_assert(std::is_trivially_copyable_v<handle>);

    class object : public handle {
    public:
        object(const object& other) : handle(other) { inc_ref(); }

        object(object&& other) noexcept : handle(other) {
            other.m_ptr = nullptr;
            other.ref_count = nullptr;
        }

        object& operator= (const object& other) {
            if(this != &other) {
                dec_ref();
                m_ptr = other.m_ptr;
                ref_count = other.ref_count;
                inc_ref();
            }
            return *this;
        }

        object& operator= (object&& other) noexcept {
            if(this != &other) {
                dec_ref();
                m_ptr = other.m_ptr;
                ref_count = other.ref_count;
                other.m_ptr = nullptr;
                other.ref_count = nullptr;
            }
            return *this;
        }

        ~object() {
            if(m_ptr != nullptr) {
                dec_ref();
            }
        }

    protected:
        object(const handle& h, bool borrow) : handle(h) {
            if(borrow) {
                inc_ref();
            }
        }

        template <typename T>
        friend T reinterpret_borrow(const handle& h) {
            return {h, true};
        }

        template <typename T>
        friend T reinterpret_steal(const handle& h) {
            return {h, false};
        }
    };

#define PYBIND11_BINARY_OPERATOR(OP, NAME)                                                         \
    template <typename Derived>                                                                    \
    inline object object_api<Derived>::operator OP (const object_api & other) const {              \
        return reinterpret_borrow<object>(vm->call(vm->py_op(NAME), this->get(), other.get()));    \
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

    PYBIND11_BINARY_OPERATOR(+=, "iadd");
    PYBIND11_BINARY_OPERATOR(-=, "isub");
    PYBIND11_BINARY_OPERATOR(*=, "imul");
    PYBIND11_BINARY_OPERATOR(/=, "itruediv");
    PYBIND11_BINARY_OPERATOR(%=, "imod");
    PYBIND11_BINARY_OPERATOR(|=, "ior");
    PYBIND11_BINARY_OPERATOR(&=, "iand");
    PYBIND11_BINARY_OPERATOR(^=, "ixor");
    PYBIND11_BINARY_OPERATOR(<<=, "ilshift");
    PYBIND11_BINARY_OPERATOR(>>=, "irshift");

    PYBIND11_BINARY_OPERATOR(==, "eq");
    PYBIND11_BINARY_OPERATOR(!=, "ne");
    PYBIND11_BINARY_OPERATOR(<, "lt");
    PYBIND11_BINARY_OPERATOR(>, "gt");
    PYBIND11_BINARY_OPERATOR(<=, "le");
    PYBIND11_BINARY_OPERATOR(>=, "ge");

#undef PYBIND11_BINARY_OPERATOR
}  // namespace pybind11
