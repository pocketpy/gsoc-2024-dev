#include "test.h"

const char* source = R"(
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Point(self.x - other.x, self.y - other.y)

    def __mul__(self, other):
        return Point(self.x * other.x, self.y * other.y)

    def __truediv__(self, other):
        return Point(self.x / other.x, self.y / other.y)

    def __floordiv__(self, other):
        return Point(self.x // other.x, self.y // other.y)

    def __mod__(self, other):
        return Point(self.x % other.x, self.y % other.y)

    def __pow__(self, other):
        return Point(self.x ** other.x, self.y ** other.y)

    def __lshift__(self, other):
        return Point(self.x << other.x, self.y << other.y)

    def __rshift__(self, other):
        return Point(self.x >> other.x, self.y >> other.y)

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __ne__(self, other) -> bool:
        return not self.__eq__(other)

    def __lt__(self, other) -> bool:
        return self.x < other.x and self.y < other.y

    def __le__(self, other) -> bool:
        return self.x <= other.x and self.y <= other.y

    def __gt__(self, other) -> bool:
        return self.x > other.x and self.y > other.y

    def __ge__(self, other) -> bool:
        return self.x >= other.x and self.y >= other.y

    def __repr__(self):
        return f'Point({self.x}, {self.y})'
)";

TEST_F(PYBIND11_TEST, object) {
    py::module_ m = py::module_::import("__main__");
    py::vm->exec(source);
    py::vm->exec("p = Point(3, 4)");
    py::handle p = py::eval("p");

    // test is
    assert(!p.is_none());
    assert(p.is(p));

    // test attrs
    assert(p.attr("x").cast<int>() == 3);
    assert(p.attr("y").cast<int>() == 4);

    p.attr("x") = py::int_(5);
    p.attr("y") = py::int_(6);

    assert(p.attr("x").cast<int>() == 5);
    assert(p.attr("y").cast<int>() == 6);
    py::vm->exec("assert p == Point(5, 6)");

    // test operators
    assert((p + p) == py::handle(py::eval("Point(10, 12)")));
}
