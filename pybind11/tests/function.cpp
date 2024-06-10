#include "test.h"

TEST_F(PYBIND11_TEST, function) {
    py::module_ m = py::module_::import("__main__");

    // test *args
    m.def("sum", [](py::args args) {
        int sum = 0;
        for(auto arg: args) {
            sum += arg.cast<int>();
        }
        return sum;
    });

    EXPECT_EQ(py::eval("sum(1, 2, 3)").cast<int>(), 6);

    // test **kwargs
    m.def("cal", [](py::kwargs kwargs) {
        int sum = kwargs["a"].cast<int>() + kwargs["b"].cast<int>() * kwargs["c"].cast<int>();
        return sum;
    });

    EXPECT_EQ(py::eval("cal(a=1, b=2, c=3)").cast<int>(), 7);
}
