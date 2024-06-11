#include "test.h"

PYBIND11_EMBEDDED_MODULE(sub, m) {
    m.def("add", [](int a, int b) {
        return a + b;
    });

    auto sub = m.def_submodule("sub2");
    sub.def("add", [](int a, int b) {
        return a + b;
    });
}

TEST_F(PYBIND11_TEST, module) {
    py::exec("import sub");
    py::print(py::eval("sub.add(1, 2)"));

    py::exec("from sub import sub2");
    py::print(py::eval("sub2.add(1, 2)"));
}
