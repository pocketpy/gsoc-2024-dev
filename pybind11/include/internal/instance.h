#pragma once

#include "kernel.h"

namespace pybind11
{
    enum class return_value_policy : uint8_t
    {
        /**
         *  This is the default return value policy, which falls back to the policy
         *  return_value_policy::take_ownership when the return value is a pointer.
         *  Otherwise, it uses return_value::move or return_value::copy for rvalue
         *  and lvalue references, respectively. See below for a description of what
         *  all of these different policies do.
         */
        automatic = 0,

        /**
         *  As above, but use policy return_value_policy::reference when the return
         *  value is a pointer. This is the default conversion policy for function
         *  arguments when calling Python functions manually from C++ code (i.e. via
         *  handle::operator()). You probably won't need to use this.
         */
        automatic_reference,

        /**
         *  Reference an existing object (i.e. do not create a new copy) and take
         *  ownership. Python will call the destructor and delete operator when the
         *  object's reference count reaches zero. Undefined behavior ensues when
         *  the C++ side does the same..
         */
        take_ownership,

        /**
         *  Create a new copy of the returned object, which will be owned by
         *  Python. This policy is comparably safe because the lifetimes of the two
         *  instances are decoupled.
         */
        copy,

        /**
         *  Use std::move to move the return value contents into a new instance
         *  that will be owned by Python. This policy is comparably safe because the
         *  lifetimes of the two instances (move source and destination) are
         *  decoupled.
         */
        move,

        /**
         *  Reference an existing object, but do not take ownership. The C++ side
         *  is responsible for managing the object's lifetime and deallocating it
         *  when it is no longer used. Warning: undefined behavior will ensue when
         *  the C++ side deletes an object that is still referenced and used by
         *  Python.
         */
        reference,

        /**
         *  This policy only applies to methods and properties. It references the
         *  object without taking ownership similar to the above
         *  return_value_policy::reference policy. In contrast to that policy, the
         *  function or property's implicit this argument (called the parent) is
         *  considered to be the the owner of the return value (the child).
         *  pybind11 then couples the lifetime of the parent to the child via a
         *  reference relationship that ensures that the parent cannot be garbage
         *  collected while Python is still using the child. More advanced
         *  variations of this scheme are also possible using combinations of
         *  return_value_policy::reference and the keep_alive call policy
         */
        reference_internal
    };

    // use to record the type information of C++ class.
    struct type_info
    {
        const char* name;
        std::size_t size;
        std::size_t alignment;
        void (*destructor)(void*);
        void (*copy)(void*, const void*);
        void (*move)(void*, void*);
        const std::type_info* type;
    };

    template <typename T>
    type_info& info_of()
    {
        static_assert(!std::is_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>,
                      "T must not be a reference type or const type.");
        static type_info info = {
            typeid(T).name(),
            sizeof(T),
            alignof(T),
            [](void* ptr) { static_cast<T*>(ptr)->~T(); },
            [](void* dst, const void* src) { ::new (dst) T(*static_cast<const T*>(src)); },
            [](void* dst, void* src) { ::new (dst) T(std::move(*static_cast<T*>(src))); },
            &typeid(T),
        };
        return info;
    }

    // all registered C++ class will be ensured as instance type.
    class instance
    {
    private:
        enum Flag
        {
            None = 0,
            Own = 1 << 0,  // if the instance is owned by C++ side.
            Ref = 1 << 1,  // need to mark the father object.
        };

        Flag flag;
        void* data;
        const type_info* type;
        pkpy::PyObject* father;

    public:
        instance() noexcept : flag(Flag::None), data(nullptr), type(nullptr), father(nullptr) {}

        instance(const instance&) = delete;

        instance(instance&& other) noexcept : flag(other.flag), data(other.data), type(other.type), father(other.father)
        {
            other.flag = Flag::None;
            other.data = nullptr;
            other.type = nullptr;
            other.father = nullptr;
        }

        template <typename T>
        static pkpy::PyObject* create(T&& value,
                                      pkpy::Type type,
                                      return_value_policy policy = return_value_policy::automatic_reference,
                                      pkpy::PyObject* father = nullptr) noexcept
        {
            using underlying_type = std::remove_cv_t<std::remove_reference_t<T>>;

            // resolve for automatic policy.
            if(policy == return_value_policy::automatic)
            {
                policy = std::is_pointer_v<underlying_type> ? return_value_policy::take_ownership
                         : std::is_lvalue_reference_v<T&&>  ? return_value_policy::copy
                                                            : return_value_policy::move;
            }
            else if(policy == return_value_policy::automatic_reference)
            {
                policy = std::is_pointer_v<underlying_type> ? return_value_policy::reference
                         : std::is_lvalue_reference_v<T&&>  ? return_value_policy::copy
                                                            : return_value_policy::move;
            }

            auto& _value = [&]() -> auto&
            {
                /**
                 * note that, pybind11 will ignore the const qualifier.
                 * in fact, try to modify a const value will result in undefined behavior.
                 */
                if constexpr(std::is_pointer_v<underlying_type>)
                {
                    return *reinterpret_cast<underlying_type*>(value);
                }
                else
                {
                    return const_cast<underlying_type&>(value);
                }
            }();

            instance instance;

            if(policy == return_value_policy::take_ownership)
            {
                instance.data = &_value;
                instance.flag = Flag::Own;
            }
            else if(policy == return_value_policy::copy)
            {
                instance.data = ::new auto(_value);
                instance.flag = Flag::Own;
            }
            else if(policy == return_value_policy::move)
            {
                instance.data = ::new auto(std::move(_value));
                instance.flag = Flag::Own;
            }
            else if(policy == return_value_policy::reference)
            {
                instance.data = &_value;
                instance.flag = Flag::None;
            }
            else if(policy == return_value_policy::reference_internal)
            {
                instance.data = &_value;
                instance.flag = Flag::Ref;
                instance.father = father;
            }

            return vm->heap.gcnew<pybind11::instance>(type, std::move(instance));
        }

        ~instance()
        {
            if(flag & Flag::Own)
            {
                type->destructor(data);
            }
        }

        void _gc_mark() const
        {
            if(father && (flag & Flag::Ref))
            {
                PK_OBJ_MARK(father);
            }
        }

        template <typename T>
        T& cast()
        {
            return *static_cast<T*>(data);
        }
    };
}  // namespace pybind11
