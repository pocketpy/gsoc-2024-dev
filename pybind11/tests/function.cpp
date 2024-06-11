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
