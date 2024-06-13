#pragma once
#include "cpp_function.h"

namespace pybind11 {

class module_ : public object {

public:
    using object::object;

    static module_ __main__() { return vm->_main; }

    static module_ import(const char* name) {
        if(name == std::string_view{"__main__"}) {
            return vm->_main;
        } else {
            return vm->py_import(name, false);
        }
    }

    module_ def_submodule(const char* name, const char* doc = nullptr) {
        const auto package = m_ptr->attr(pkpy::__package__).obj_get<pkpy::Str>() + "." +
                             m_ptr->attr(pkpy::__name__).obj_get<pkpy::Str>();
        auto m = vm->new_module(name, package);
        m_ptr->attr().set(name, m);
        return m;
    }

    template <typename Fn, typename... Extras>
    module_& def(const char* name, Fn&& fn, const Extras... extras) {
        impl::bind_function(*this, name, std::forward<Fn>(fn), pkpy::BindType::DEFAULT, extras...);
        return *this;
    }
};

#define PYBIND11_EMBEDDED_MODULE(name, variable)                                                                       \
    static void _pybind11_register_##name(pybind11::module_& variable);                                                \
    namespace pybind11::impl {                                                                                         \
    auto _module_##name = [] {                                                                                         \
        if(_init == nullptr) _init = new std::vector<void (*)()>();                                                    \
        _init->push_back([] {                                                                                          \
            pybind11::module_ m = vm->new_module(#name, "");                                                           \
            _pybind11_register_##name(m);                                                                              \
        });                                                                                                            \
        return 1;                                                                                                      \
    }();                                                                                                               \
    }                                                                                                                  \
    static void _pybind11_register_##name(pybind11::module_& variable)

}  // namespace pybind11
