#pragma once

#include "function.h"

namespace pkbind {

class module : public object {
    PKBIND_TYPE_IMPL(object, tp_module)

    static module __main__() { return module(py_getmodule("__main__"), object::ref_t{}); }

    static module import(const char* name) {
        raise_call<py_import>(name);
        return module(py_retval(), object::realloc_t{});
        // if(name == std::string_view{"__main__"}) {
        //     return vm->_main;
        // } else {
        //     return vm->py_import(name, false);
        // }
    }

    // module def_submodule(const char* name, const char* doc = nullptr) {
    //      TODO: resolve package
    //      auto package = this->package()._as<pkpy::Str>() + "." + this->name()._as<pkpy::Str>();
    //      auto fname = this->name()._as<pkpy::Str>() + "." + name;
    //      auto m = vm->new_module(fname, "");
    //      setattr(*this, name, m);
    //      return m;
    // }

    template <typename Fn, typename... Extras>
    module& def(const char* name, Fn&& fn, const Extras... extras) {
        impl::bind_function<false, false>(*this, name, std::forward<Fn>(fn), extras...);
        return *this;
    }
};

using module_ = module;

}  // namespace pkbind
