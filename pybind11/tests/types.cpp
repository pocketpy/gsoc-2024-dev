#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

void test_int() {
    py::object obj = py::int_(123);
    py::handle obj2 = py::vm->eval("123");

    assert(obj == obj2);

    assert(obj.cast<int>() == 123);
    assert(obj.cast<long>() == 123);
    assert(obj.cast<long long>() == 123);
}

void test_float() {
    py::object obj = py::float_(123.0);
    py::handle obj2 = py::vm->eval("123.0");

    assert(obj == obj2);

    assert(obj.cast<float>() == 123.0);
    assert(obj.cast<double>() == 123.0);
    // assert(obj.cast<long double>() == 123.0);
}

void test_str() {
    py::object obj = py::str("123");
    py::handle obj2 = py::vm->eval("'123'");

    assert(obj == obj2);

    assert(obj.cast<const char*>() == std::string_view{"123"});
    assert(obj.cast<char*>() == std::string_view{"123"});
    assert(obj.cast<std::string>() == "123");
    assert(obj.cast<std::string_view>() == "123");
}

int test_types() {
    py::initialize();
    test_int();
    test_float();
    test_str();
    py::finalize();
    return 0;
}
