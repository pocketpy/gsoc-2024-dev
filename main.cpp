#include <pybind11/pybind11.h>

namespace py = pkbind;

int main() {
    py::scoped_interpreter guard{};
    auto m = py_newmodule("example");
    auto n = py::module::import("example");

    py::tuple p = {py::object()};
}
