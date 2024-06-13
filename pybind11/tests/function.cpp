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
