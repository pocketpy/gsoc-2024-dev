#pragma once
#include "types.h"
#include "type_traits.h"

namespace pybind11 {

inline object eval(std::string_view code, const handle& global = none{}, handle local = none{}) {
#if PK_VERSION_MAJOR == 2
    return vm->py_eval(code, global.ptr(), local.ptr());
#else
    return vm->eval(code);
#endif
}

inline void exec(std::string_view code, const handle& global = none{}, handle local = none{}) {
#if PK_VERSION_MAJOR == 2
    vm->py_exec(code, global.ptr(), local.ptr());
#else
    vm->exec(code);
#endif
}

/// globas() in pkpy is immutable, your changes will not be reflected in the Python interpreter
inline dict globals() {
    auto& proxy = eval("globals()")._as<pkpy::MappingProxy>().attr();
    dict result;
#if PK_VERSION_MAJOR == 2
    proxy.apply(
        [](pkpy::StrName key, pkpy::PyVar value, void* data) {
            auto& dict = static_cast<pybind11::dict*>(data)->_as<pkpy::Dict>();
            auto key_ = pybind11::str(key.sv()).ptr();
            dict.set(vm, key_, value);
        },
        &result);
#else
    proxy.apply([&](pkpy::StrName key, pkpy::PyVar value) {
        result.setitem(str(key.sv()), value);
    });
#endif
    return result;
}

// wrapper for builtin functions in Python
inline bool hasattr(const handle& obj, const handle& name) {
    return vm->getattr(obj.ptr(), name._as<pkpy::Str>(), false) != nullptr;
}

inline bool hasattr(const handle& obj, const char* name) { return vm->getattr(obj.ptr(), name, false) != nullptr; }

inline void delattr(const handle& obj, const handle& name) { vm->delattr(obj.ptr(), name._as<pkpy::Str>()); }

inline void delattr(const handle& obj, const char* name) { vm->delattr(obj.ptr(), name); }

inline object getattr(const handle& obj, const handle& name) { return vm->getattr(obj.ptr(), name._as<pkpy::Str>()); }

inline object getattr(const handle& obj, const char* name) { return vm->getattr(obj.ptr(), name); }

inline object getattr(const handle& obj, const handle& name, const handle& default_) {
    auto attr = vm->getattr(obj.ptr(), name._as<pkpy::Str>(), false);
    if(attr) { return attr; }
    return default_;
}

inline object getattr(const handle& obj, const char* name, const handle& default_) {
    auto attr = vm->getattr(obj.ptr(), name, false);
    if(attr) { return attr; }
    return default_;
}

inline void setattr(const handle& obj, const handle& name, const handle& value) {
    vm->setattr(obj.ptr(), name._as<pkpy::Str>(), value.ptr());
}

inline void setattr(const handle& obj, const char* name, const handle& value) {
    vm->setattr(obj.ptr(), name, value.ptr());
}

template <typename T>
inline bool isinstance(const handle& obj) {
    return type_visitor::check<T>(obj);
}

template <>
inline bool isinstance<handle>(const handle&) = delete;

inline bool isinstance(const handle& obj, const handle& type) {
    return vm->isinstance(obj.ptr(), type._as<pkpy::Type>());
}

inline int64_t hash(const handle& obj) { return vm->py_hash(obj.ptr()); }

template <typename T, typename SFINAE = void>
struct type_caster;

template <typename T>
handle cast(T&& value, return_value_policy policy, handle parent) {
    // decay_t can resolve c-array type, but remove_cv_ref_t can't.
    using underlying_type = std::decay_t<T>;

    if constexpr(std::is_convertible_v<underlying_type, handle>) {
        return std::forward<T>(value);
    } else {
        static_assert(!is_multiple_pointer_v<underlying_type>, "multiple pointer is not supported.");
        static_assert(!std::is_void_v<std::remove_pointer_t<underlying_type>>,
                      "void* is not supported, consider using py::capsule.");

        // resolve for automatic policy.
        if(policy == return_value_policy::automatic) {
            policy = std::is_pointer_v<underlying_type> ? return_value_policy::take_ownership
                     : std::is_lvalue_reference_v<T&&>  ? return_value_policy::copy
                                                        : return_value_policy::move;
        } else if(policy == return_value_policy::automatic_reference) {
            policy = std::is_pointer_v<underlying_type> ? return_value_policy::reference
                     : std::is_lvalue_reference_v<T&&>  ? return_value_policy::copy
                                                        : return_value_policy::move;
        }

        return type_caster<underlying_type>::cast(std::forward<T>(value), policy, parent);
    }
}

template <typename T>
T cast(const handle& obj, bool convert) {
    assert(obj.ptr() != nullptr);

    type_caster<T> caster = {};

    if(caster.load(obj, convert)) {
        return caster.value;
    } else {
        std::string msg = "cast python instance to c++ failed, ";
        msg += "obj type is: {";
        msg += type::of(obj).name();
        msg += "}, target type is: {";
        msg += type_name<T>();
        msg += "}.";
        vm->TypeError(msg);
        PK_UNREACHABLE();
    }
}

template <typename T>
T handle::cast() const {
    return pybind11::cast<T>(*this);
}

template <typename... Args>
void print(Args&&... args) {
    pkpy::PyVar print = vm->builtins->attr("print");
    vm->call(print, cast(std::forward<Args>(args)).ptr()...);
}

}  // namespace pybind11
