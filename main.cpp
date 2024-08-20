#include <pybind11/pybind11.h>

namespace py = pkbind;

int main() {
    {
        // py::scoped_interpreter guard{};
        // py::dict locals = {py::arg("x") = 1, py::arg("y") = 2};
        // py::object obj = py::eval("x + y", py::none(), locals);
        py_initialize();
        py_resetvm();
    }

    //  py::finalize();

    // py::initialize();
}
