#include "pybind.h"

using namespace pkpy;

struct Point {
  int x;
  int y;

  Point(int x, int y) : x(x), y(y) {}

  int sum(int z) { return x + y + z; }
};

int main() {
  // Create a virtual machine
  VM *vm = new VM();

  // clang-format off
    module_ m{vm, vm->_main};
    class_<Point>(m, "Point")
        .def(init<int, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y)
        .def("sum", &Point::sum);
  // clang-format on

  vm->exec("a = Point(1, 2)");
  vm->exec("print(a.x)");      // 1
  vm->exec("print(a.y)");      // 2
  vm->exec("print(a.sum(3))"); // 6

  delete vm;
  return 0;
}
