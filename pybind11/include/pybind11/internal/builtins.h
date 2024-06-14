#pragma once
#include "types.h"
#include "type_traits.h"

namespace pybind11 {

inline object eval(std::string_view code, const handle& global = none{}, handle local = none{}) {
    return vm->py_eval(code, global.ptr(), local.ptr());
}

inline void exec(std::string_view code, const handle& global = none{}, handle local = none{}) {
    vm->py_exec(code, global.ptr(), local.ptr());
}

/// globas() in pkpy is immutable, your changes will not be reflected in the Python interpreter
inline dict globals() {
    auto& proxy = eval("globals()")._as<pkpy::MappingProxy>().attr();
    dict result;
    proxy.apply(
        [](pkpy::StrName key, pkpy::PyVar value, void* data) {
            auto& dict = static_cast<pybind11::dict*>(data)->_as<pkpy::Dict>();
            auto key_ = pybind11::str(key.sv()).ptr();
            dict.set(vm, key_, value);
        },
        &result);
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
    if constexpr(std::is_convertible_v<remove_cvref_t<T>, handle>) {
        return std::forward<T>(value);
    } else {
        return type_caster<std::decay_t<T>>::cast(std::forward<T>(value), policy, parent);
    }
}

template <typename T>
T cast(const handle& obj, bool convert) {
    if(!obj) { vm->TypeError("Unable to cast null handle to C++ type."); }

    type_caster<T> caster;

    if(caster.load(obj, convert)) { return caster.value; }

    // TODO: improve error message
    std::string msg = "Unable to cast Python instance to C++ type.";
    // msg += " obj type: ";
    // print(type::of(obj).name());
    // msg += type::of(obj).name();
    // msg += ", target type: ";
    // msg += type_name<T>();
    // msg += ".";
    print(msg);
    vm->TypeError(msg);
    // print("objtype::of(obj));
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
