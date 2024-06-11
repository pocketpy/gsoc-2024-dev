#include "test.h"

TEST_F(PYBIND11_TEST, int) {
    py::object obj = py::int_(123);
    py::handle obj2 = py::eval("123");

    EXPECT_EQ(obj, obj2);

    EXPECT_EQ(obj.cast<int>(), 123);
    EXPECT_EQ(obj.cast<long>(), 123);
    EXPECT_EQ(obj.cast<long long>(), 123);
}

TEST_F(PYBIND11_TEST, float) {
    py::object obj = py::float_(123.0);
    py::handle obj2 = py::eval("123.0");

    EXPECT_EQ(obj, obj2);

    EXPECT_EQ(obj.cast<float>(), 123.0);
    EXPECT_EQ(obj.cast<double>(), 123.0);
}

TEST_F(PYBIND11_TEST, str) {
    py::object obj = py::str("123");
    py::handle obj2 = py::eval("'123'");

    EXPECT_EQ(obj, obj2);

    EXPECT_STREQ(obj.cast<char*>(), "123");
    EXPECT_STREQ(obj.cast<const char*>(), "123");
    EXPECT_EQ(obj.cast<std::string>(), "123");
    EXPECT_EQ(obj.cast<std::string_view>(), "123");
}

TEST_F(PYBIND11_TEST, tuple) {
    py::tuple tuple = py::tuple(1, "123", 3);
    EXPECT_EQ(tuple, py::eval("(1, '123', 3)"));
    EXPECT_EQ(tuple.size(), 3);
    EXPECT_FALSE(tuple.empty());

    tuple[0] = py::int_(3);
    tuple[2] = py::int_(1);
    EXPECT_EQ(tuple, py::eval("(3, '123', 1)"));
}

TEST_F(PYBIND11_TEST, list) {
    // test constructors
    py::list list = py::list();
    EXPECT_EQ(list, py::eval("[]"));
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());

    list = py::list(1, 2, 3);
    EXPECT_EQ(list, py::eval("[1, 2, 3]"));
    EXPECT_EQ(list.size(), 3);
    EXPECT_FALSE(list.empty());

    // test accessor
    list[0] = py::int_(3);
    list[2] = py::int_(1);
    EXPECT_EQ(list, py::eval("[3, 2, 1]"));

    // test other apis
    list.append(py::int_(4));
    EXPECT_EQ(list, py::eval("[3, 2, 1, 4]"));

    list.extend(py::list(5, 6));
    EXPECT_EQ(list, py::eval("[3, 2, 1, 4, 5, 6]"));

    list.insert(0, py::int_(7));
    EXPECT_EQ(list, py::eval("[7, 3, 2, 1, 4, 5, 6]"));
}

TEST_F(PYBIND11_TEST, dict) {
    // test constructors
    py::dict dict = py::dict();
    EXPECT_EQ(dict, py::eval("{}"));
    EXPECT_EQ(dict.size(), 0);
    EXPECT_TRUE(dict.empty());

    // test accessor
    dict["a"] = py::int_(1);
    dict["b"] = py::int_(2);
    dict["c"] = py::int_(3);
    EXPECT_EQ(dict, py::eval("{'a': 1, 'b': 2, 'c': 3}"));

    // test other apis
    dict.clear();
    EXPECT_EQ(dict, py::eval("{}"));
}

TEST_F(PYBIND11_TEST, capsule) {
    static int times = 0;

    struct NotTrivial {
        ~NotTrivial() { times++; }
    };

    NotTrivial obj;
    py::handle x = py::capsule(obj);

    py::finalize();
    EXPECT_EQ(times, 1);
}
