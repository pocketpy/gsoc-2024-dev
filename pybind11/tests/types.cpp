#include <pybind11/pybind11.h>

namespace py = pybind11;

void test_int() {
    py::object obj = py::int_(123);
    py::handle obj2 = py::eval("123");

    assert(obj == obj2);

    assert(obj.cast<int>() == 123);
    assert(obj.cast<long>() == 123);
    assert(obj.cast<long long>() == 123);
}

void test_float() {
    py::object obj = py::float_(123.0);
    py::handle obj2 = py::eval("123.0");

    assert(obj == obj2);

    assert(obj.cast<float>() == 123.0);
    assert(obj.cast<double>() == 123.0);
}

void test_str() {
    py::object obj = py::str("123");
    py::handle obj2 = py::eval("'123'");

    assert(obj == obj2);

    assert(obj.cast<const char*>() == std::string_view{"123"});
    assert(obj.cast<char*>() == std::string_view{"123"});
    assert(obj.cast<std::string>() == "123");
    assert(obj.cast<std::string_view>() == "123");
}

void test_tuple() {
    py::tuple tuple = py::tuple(1, "123", 3);
    assert(tuple == py::eval("(1, '123', 3)"));
    assert(tuple.size() == 3);
    assert(!tuple.empty());

    tuple[0] = py::int_(3);
    tuple[2] = py::int_(1);
    assert(tuple == py::eval("(3, '123', 1)"));
}

void test_list() {
    // test constructors
    py::list list = py::list();
    assert(list == py::eval("[]"));
    assert(list.size() == 0);
    assert(list.empty());

    list = py::list(1, 2, 3);
    assert(list == py::eval("[1, 2, 3]"));
    assert(list.size() == 3);
    assert(!list.empty());

    // test accessor
    list[0] = py::int_(3);
    list[2] = py::int_(1);
    assert(list == py::eval("[3, 2, 1]"));

    // test other apis
    list.append(py::int_(4));
    assert(list == py::eval("[3, 2, 1, 4]"));

    list.extend(py::list(5, 6));
    assert(list == py::eval("[3, 2, 1, 4, 5, 6]"));

    list.insert(0, py::int_(7));
    assert(list == py::eval("[7, 3, 2, 1, 4, 5, 6]"));
}

int test_types() {
    py::initialize();
    try {
        test_int();
        test_float();
        test_str();
        test_tuple();
        test_list();
    } catch(const std::exception& e) { py::print(e.what()); }
    return 0;
}
