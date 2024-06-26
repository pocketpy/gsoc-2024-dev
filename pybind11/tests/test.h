#include <gtest/gtest.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

class PYBIND11_TEST : public ::testing::Test {
protected:
    void SetUp() override { py::interpreter::initialize(); }

    void TearDown() override { py::interpreter::finalize(); }
};

#define EXPECT_CAST_EQ(expr, expected) EXPECT_EQ(py::cast(expr), py::cast(expected))
#define EXPECT_EVAL_EQ(expr, expected) EXPECT_EQ(py::eval(expr).cast<decltype(expected)>(), expected)
#define EXPECT_EXEC_EQ(expr, expected) EXPECT_EQ(py::eval(expr), py::eval(expected))
