#include <gtest/gtest.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

class PYBIND11_TEST : public ::testing::Test {
protected:
    void SetUp() override { py::initialize(); }

    void TearDown() override {
        if(py::vm) { py::finalize(); }
    }
};
