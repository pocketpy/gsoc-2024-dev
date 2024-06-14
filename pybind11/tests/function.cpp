#include "test.h"

TEST_F(PYBIND11_TEST, args) {
    auto m = py::module_::__main__();

    // test for binding function with args
    m.def("sum", [](py::args args) {
        int sum = 0;
        for(auto arg: args) {
            sum += arg.cast<int>();
        }
        return sum;
    });

    EXPECT_EVAL_EQ("sum(1, 2, 3)", 6);
}

TEST_F(PYBIND11_TEST, kwargs) {
    auto m = py::module_::__main__();

    // test for binding function with kwargs
    m.def("cal", [](py::kwargs kwargs) {
        int sum = kwargs["a"].cast<int>() + kwargs["b"].cast<int>() * kwargs["c"].cast<int>();
        return sum;
    });

    EXPECT_EVAL_EQ("cal(a=1, b=2, c=3)", 7);
}

TEST_F(PYBIND11_TEST, return_value_policy) {
    static int copy_constructor_calls = 0;
    static int move_constructor_calls = 0;
    static int destructor_calls = 0;

    struct Point {
        int x, y;

        Point(int x, int y) : x(x), y(y) {}

        Point(const Point& p) : x(p.x), y(p.y) { copy_constructor_calls++; }

        Point(Point&& p) : x(p.x), y(p.y) { move_constructor_calls++; }

        ~Point() { destructor_calls++; }

        static Point& make_point() { return *new Point(1, 2); }

        bool operator== (const Point& p) const { return x == p.x && y == p.y; }
    };

    auto test = [](py::return_value_policy policy, auto fn) {
        py::initialize();
        copy_constructor_calls = 0;
        move_constructor_calls = 0;
        destructor_calls = 0;

        auto m = py::module_::__main__();

        py::class_<Point>(m, "Point")
            .def(py::init<int, int>())
            .def_readwrite("x", &Point::x)
            .def_readwrite("y", &Point::y)
            .def("__eq__", &Point::operator==);

        m.def("make_point", Point::make_point, policy);

        EXPECT_EVAL_EQ("make_point()", Point::make_point());

        py::finalize();

        fn();
    };

    test(py::return_value_policy::reference, []() {
        EXPECT_EQ(copy_constructor_calls, 0);
        EXPECT_EQ(move_constructor_calls, 0);
        EXPECT_EQ(destructor_calls, 0);
    });

    test(py::return_value_policy::copy, []() {
        EXPECT_EQ(copy_constructor_calls, 1);
        EXPECT_EQ(move_constructor_calls, 0);
        EXPECT_EQ(destructor_calls, 1);
    });

    test(py::return_value_policy::move, []() {
        EXPECT_EQ(copy_constructor_calls, 0);
        EXPECT_EQ(move_constructor_calls, 1);
        EXPECT_EQ(destructor_calls, 1);
    });

    test(py::return_value_policy::take_ownership, []() {
        EXPECT_EQ(copy_constructor_calls, 0);
        EXPECT_EQ(move_constructor_calls, 0);
        EXPECT_EQ(destructor_calls, 1);
    });
}

TEST_F(PYBIND11_TEST, lambda) {
    auto m = py::module_::__main__();

    static int destructor_calls = 0;

    struct NotTrivial {
        int data;

        int operator() (int x, int y) { return x + y + data; }

        ~NotTrivial() { destructor_calls++; }
    };

    struct NotSmall {
        size_t a;
        size_t b;
        size_t c;
        size_t d;

        int operator() (int x, int y) { return x + y + a + b + c + d; }

        ~NotSmall() { destructor_calls++; }
    };

    // test for binding lambda
    m.def("cal", NotTrivial{3});
    m.def("cal2", NotSmall{3, 4, 5, 6});

    EXPECT_EVAL_EQ("cal(1, 2)", 6);
    EXPECT_EVAL_EQ("cal2(1, 2)", 21);

    py::finalize();

    EXPECT_EQ(destructor_calls, 4);
}
