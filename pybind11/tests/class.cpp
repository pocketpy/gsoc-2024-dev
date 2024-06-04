#include <iostream>
#include <pybind11/pybind11.h>

// test for simple struct, all member is built-in type.
struct Point {
    int x;
    int y;

private:
    int z;

public:
    inline static int constructor_calls = 0;
    inline static int copy_constructor_calls = 0;
    inline static int move_constructor_calls = 0;
    inline static int destructor_calls = 0;

    Point() : x(0), y(0), z(0) { constructor_calls++; }

    Point(int x, int y, int z) : x(x), y(y), z(z) { constructor_calls++; }

    Point(const Point& p) : x(p.x), y(p.y) {
        copy_constructor_calls++;
        constructor_calls++;
    }

    Point(Point&& p) noexcept : x(p.x), y(p.y) {
        move_constructor_calls++;
        constructor_calls++;
    }

    Point& operator= (const Point& p) {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    ~Point() { destructor_calls++; }

    int& get_z() { return z; }

    void set_z(int z) { this->z = z; }

    std::string stringfy() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

struct Line {
    Point start;
    Point end;
};

namespace py = pybind11;

void test_simple() {
    py::module_ m = py::module_::import("__main__");
    py::class_<Point>(m, "Point")
        .def(py::init<>())
        .def(py::init<int, int, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def_property("z", &Point::get_z, &Point::set_z)
        .def("stringfy", &Point::stringfy);

    py::vm->exec(R"(
p = Point()
assert p.stringfy() == '(0, 0, 0)'
p = Point(1, 2, 3)
assert p.x == 1
assert p.y == 2
assert p.z == 3
assert p.stringfy() == '(1, 2, 3)'
p.x = 10
p.y = 20
p.z = 30
assert p.stringfy() == '(10, 20, 30)'
)");
}

void test_complex() {
    py::module_ m = py::module_::import("__main__");
    py::class_<Line> line(m, "Line");

    line  // not bind constructor
        .def_readwrite("start", &Line::start)
        .def_readwrite("end", &Line::end);

    try {
        py::vm->eval("Line()");
    } catch(const std::exception& e) {
        // expect error: "Bound class must have constructor"
        std::cerr << e.what() << std::endl;
    }

    // bind constructor
    line.def(py::init<>());

    py::vm->exec(R"(
l = Line()
l.start = Point(1, 2, 3)
l.end = Point(4, 5, 6)
p = l.start
assert l.start.stringfy() == '(1, 2, 3)'
assert l.end.stringfy() == '(4, 5, 6)'
)");
}

int test_class() {
    py::initialize();

    try {
        test_simple();
        test_complex();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    py::finalize();
    assert(Point::constructor_calls == Point::destructor_calls);
    assert(Point::copy_constructor_calls == 0);
    assert(Point::move_constructor_calls == 0);

    return 0;
}
