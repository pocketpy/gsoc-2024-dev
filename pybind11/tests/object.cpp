#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

int test_object(){
    py::initialize();
    try{
        py::module_ m = py::module_::import("__main__");
        py::handle str = py::vm->eval("123");
        assert(str == py::int_(123));
    }catch(const std::exception &e){
        std::cerr << e.what() << std::endl;
    }
    return 0;
}