#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace pybind11;

int main() {
    py::scoped_interpreter guard{};
    py::exec("print('Hello, World!')");
    return 0;
}
