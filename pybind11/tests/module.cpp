#include "test.h"

PYBIND11_EMBEDDED_MODULE(sub, m) {
    m.def("add", [](int a, int b) {
        return a + b;
    });

    auto sub = m.def_submodule("sub2");
    sub.def("add", [](int a, int b) {
        return a + b;
    });
}

TEST_F(PYBIND11_TEST, module) {
    py::exec("import sub");
    EXPECT_EVAL_EQ("sub.add(1, 2)", 3);

    py::exec("from sub import sub2");
    EXPECT_EVAL_EQ("sub2.add(1, 2)", 3);
}
