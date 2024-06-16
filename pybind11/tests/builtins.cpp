#include "test.h"

namespace {

int copy_constructor_calls = 0;
int move_constructor_calls = 0;
int destructor_calls = 0;

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}

    Point(const Point& other) : x(other.x), y(other.y) { ++copy_constructor_calls; }

    Point(Point&& other) : x(other.x), y(other.y) { ++move_constructor_calls; }

    ~Point() { ++destructor_calls; }

    bool operator== (const Point& p) const { return x == p.x && y == p.y; }
};

}  // namespace

TEST_F(PYBIND11_TEST, cpp_cast_py) {
    auto m = py::module_::__main__();

    py::class_<Point>(m, "Point")
        .def(py::init<int, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def("__eq__", &Point::operator==);

    Point p(1, 2);

    // for py::cast's default policy

    // if argument is lvalue, policy is copy
    py::object o = py::cast(p);
    EXPECT_EQ(py::cast<Point&>(o), p);
    EXPECT_EQ(copy_constructor_calls, 1);
    EXPECT_EQ(move_constructor_calls, 0);

    // if argument is rvalue, policy is move
    py::object o2 = py::cast(std::move(p));
    EXPECT_EQ(py::cast<Point&>(o2), p);
    EXPECT_EQ(copy_constructor_calls, 1);
    EXPECT_EQ(move_constructor_calls, 1);

    // if argument is pointer, policy is reference(no taking ownership)
    py::object o3 = py::cast(&p);
    EXPECT_EQ(py::cast<Point&>(o3), p);
    EXPECT_EQ(copy_constructor_calls, 1);
    EXPECT_EQ(move_constructor_calls, 1);

    py::finalize();

    EXPECT_EQ(destructor_calls, 2);
}
