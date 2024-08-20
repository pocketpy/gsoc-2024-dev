#include <pybind11/pybind11.h>

namespace py = pkbind;

int main() {
    py::scoped_interpreter guard{};
    py::dict dict = {py::arg("a") = 1, py::arg("2") = 3};
    py::object items = dict.attr("items")();

    for(auto begin = items.begin(), end = items.end(); begin != end; ++begin) {
        py::tuple pair = py::tuple(*begin, py::object::ref_t{});
        py::print(pair[0], pair[1]);
    }

    py::print(py::str(std::string("123")) == py::eval("'123'"));
}
