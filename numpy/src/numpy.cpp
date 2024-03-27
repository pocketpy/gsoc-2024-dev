#include "numpy.hpp"

namespace pkpy{
    void add_numpy_module(VM* vm){
        PyObject* mod = vm->new_module("numpy");
        
        vm->bind(mod, "hello()", [](VM* vm, ArgsView args){
            std::cout << "Hello from numpy!" << std::endl;
            return vm->None;
        });
    }
}