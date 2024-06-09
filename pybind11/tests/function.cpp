#include <pybind11/pybind11.h>

namespace py = pybind11;

int test_function() {
    py::initialize();
    try {
        py::module_ m = py::module_::import("__main__");
        // m.def("sum", [](py::args args) {
        //
        //
        //    int sum = 0;
        //    for(auto arg: args) {
        //        sum += arg.cast<int>();
        //    }
        //    return sum;
        //});
        //py::exec("assert sum(1, 2, 3) == 6");

    } catch(const std::exception& e) { py::print(e.what()); }

    py::finalize();
    return 0;
}
