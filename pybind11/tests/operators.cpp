#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

struct Int {
    int x;

    Int(int x) : x(x) {}

    bool operator== (const Int& other) const { return x == other.x; }

    bool operator!= (const Int& other) const { return x != other.x; }

    bool operator< (const Int& other) const { return x < other.x; }

    bool operator<= (const Int& other) const { return x <= other.x; }

    bool operator> (const Int& other) const { return x > other.x; }

    bool operator>= (const Int& other) const { return x >= other.x; }

    bool operator!() const { return !x; }
};

int test_operators() {
    py::initialize();

    try {
        py::module_ m = py::module_::import("__main__");
        py::class_<Int>(m, "Int")
            .def(py::init<int>())
            .def_readwrite("x", &Int::x)
            .def("__eq__", &Int::operator==)
            .def("__ne__", &Int::operator!=)
            .def("__lt__", &Int::operator<)
            .def("__le__", &Int::operator<=)
            .def("__gt__", &Int::operator>)
            .def("__ge__", &Int::operator>=);

        py::exec(R"(
a = Int(1)
b = Int(2)
assert a < b
assert a <= b
assert a != b
assert a == a
assert a >= a
assert not a > a
assert not not a
        )");

    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    py::finalize();
    return 0;
}
