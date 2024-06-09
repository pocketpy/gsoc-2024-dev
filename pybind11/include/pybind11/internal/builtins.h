#pragma once

#include "types.h"
#include "type_traits.h"

namespace pybind11 {
    inline object
        eval(std::string_view code, const handle& global = none{}, handle local = none{}) {
        return reinterpret_borrow<object>(vm->py_eval(code, global.ptr(), local.ptr()));
    }

    inline void exec(std::string_view code, const handle& global = none{}, handle local = none{}) {
        vm->py_exec(code, global.ptr(), local.ptr());
    }

    // wrapper for builtin functions in Python
    inline bool hasattr(const handle& obj, const handle& name) {
        auto& key = _builtin_cast<pkpy::Str>(name);
        return vm->getattr(obj.ptr(), key, false) != nullptr;
    }

    inline bool hasattr(const handle& obj, const char* name) {
        return vm->getattr(obj.ptr(), name, false) != nullptr;
    }

    inline void delattr(const handle& obj, const handle& name) {
        auto& key = _builtin_cast<pkpy::Str>(name);
        vm->delattr(obj.ptr(), key);
    }

    inline void delattr(const handle& obj, const char* name) { vm->delattr(obj.ptr(), name); }

    inline object getattr(const handle& obj, const handle& name) {
        auto& key = _builtin_cast<pkpy::Str>(name);
        return reinterpret_borrow<object>(vm->getattr(obj.ptr(), key));
    }

    inline object getattr(const handle& obj, const char* name) {
        return reinterpret_borrow<object>(vm->getattr(obj.ptr(), name));
    }

    inline object getattr(const handle& obj, const handle& name, const handle& default_) {
        if(!hasattr(obj, name)) {
            return reinterpret_borrow<object>(default_);
        }
        return getattr(obj, name);
    }

    inline object getattr(const handle& obj, const char* name, const handle& default_) {
        if(!hasattr(obj, name)) {
            return reinterpret_borrow<object>(default_);
        }
        return getattr(obj, name);
    }

    inline void setattr(const handle& obj, const handle& name, const handle& value) {
        auto& key = _builtin_cast<pkpy::Str>(name);
        vm->setattr(obj.ptr(), key, value.ptr());
    }

    inline void setattr(const handle& obj, const char* name, const handle& value) {
        vm->setattr(obj.ptr(), name, value.ptr());
    }

    inline void setitem(const handle& obj, const handle& key, const handle& value) {
        vm->call_method(obj.ptr(), pkpy::__setitem__, key.ptr(), value.ptr());
    }

    template <typename T>
    inline bool isinstance(const handle& obj) {
        if constexpr(std::is_same_v<T, iterable>) {
            // if the object has __iter__ attribute, it is iterable
            return !vm->getattr(obj.ptr(), pkpy::__iter__, false);
        } else if constexpr(std::is_same_v<T, iterator>) {
            // if the object has __iter__ and __next__ attribute, it is iterator
            return !vm->getattr(obj.ptr(), pkpy::__iter__, false) &&
                   !vm->getattr(obj.ptr(), pkpy::__next__, false);
        } else {
            pkpy::Type cls = _builtin_cast<pkpy::Type>(type::handle_of<T>().ptr());
            return vm->isinstance(obj.ptr(), cls);
        }
    }

    template <>
    inline bool isinstance<handle>(const handle&) = delete;

    inline bool isinstance(const handle& obj, const handle& type) {
        return vm->isinstance(obj.ptr(), _builtin_cast<pkpy::Type>(type));
    }

    inline int64_t hash(const handle& obj) { return vm->py_hash(obj.ptr()); }

    template <typename T, typename SFINAE = void>
    struct type_caster;

    template <typename T>
    handle _cast(T&& value,
                 return_value_policy policy = return_value_policy::automatic_reference,
                 handle parent = handle()) {
        if constexpr(std::is_convertible_v<remove_cvref_t<T>, handle>) {
            return std::forward<T>(value);
        } else {
            return type_caster<std::decay_t<T>>::cast(std::forward<T>(value), policy, parent);
        }
    }

    template <typename T>
    object cast(T&& value,
                return_value_policy policy = return_value_policy::automatic_reference,
                handle parent = handle()) {
        return reinterpret_borrow<object>(_cast(std::forward<T>(value), policy, parent));
    }

    template <typename T>
    T cast(handle obj, bool convert = false) {

        type_caster<T> caster;

        if(caster.load(obj, convert)) {
            return caster.value;
        }

        throw std::runtime_error("Unable to cast Python instance to C++ type");
    }

    template <typename T>
    T handle::cast() const {
        return pybind11::cast<T>(*this);
    }

    template <typename... Args>
    inline void print(Args&&... args) {
        pkpy::PyVar print = vm->builtins->attr("print");
        vm->call(print, _cast(std::forward<Args>(args)).ptr()...);
    }
}  // namespace pybind11
