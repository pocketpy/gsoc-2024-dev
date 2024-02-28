#include "pybind.h"

using namespace pkpy;
using namespace pkpy::cxx_bind;

struct Point {
  int x;
  int y;

  Point(int x) : x(x), y(100) {}

  Point(int x, int y) : x(x), y(y) {}

  int sum(int z) { return x + y + z; }

  int sum(int a, int b) { return x + y + a + b; }

  int sub() { return x - y; }

  int sub(int z) { return x + y - z; }
};

void register_point(module_ &m) {
  class_<Point>(m, "Point")
      .def(init<int>())
      .def(init<int, int>())
      .def_readwrite("x", &Point::x)
      .def_readwrite("y", &Point::y)
      .def("sum", static_cast<int (Point::*)(int)>(&Point::sum))
      .def("sum", static_cast<int (Point::*)(int, int)>(&Point::sum))
      .def("sub", static_cast<int (Point::*)()>(&Point::sub))
      .def("sub", static_cast<int (Point::*)(int)>(&Point::sub));
}

void test_for_point() {
  std::cout << "-----------------------------------" << std::endl;
  VM *vm = new VM();
  module_ m{vm, vm->_main};
  register_point(m);

  vm->exec("a = Point(1, 2)");
  vm->exec("print(a.x)");         // 1
  vm->exec("print(a.y)");         // 2
  vm->exec("print(a.sum(3))");    // 6
  vm->exec("print(a.sum(3, 4))"); // 10
  vm->exec("print(a.sub())");     // -1
  vm->exec("print(a.sub(3))");    // 0

  vm->exec("a = Point(1)");
  vm->exec("print(a.x)");         // 1
  vm->exec("print(a.y)");         // 100
  vm->exec("print(a.sum(3))");    // 104
  vm->exec("print(a.sum(3, 4))"); // 108
  vm->exec("print(a.sub())");     // -99
  vm->exec("print(a.sub(3))");    // 98

  delete vm;
}

enum class Color {
  RED,
  GREEN,
  BLUE,
  YELLOW

};

void register_enum(module_ &m) {
  enum_<Color>(m, "Color")
      .value("RED", Color::RED)
      .value("GREEN", Color::GREEN)
      .value("BLUE", Color::BLUE)
      .value("YELLOW", Color::YELLOW);
}

void test_for_enum() {
  std::cout << "-----------------------------------" << std::endl;
  VM *vm = new VM();
  module_ m{vm, vm->_main};
  register_enum(m);

  vm->exec("print(Color.RED)");    // 0
  vm->exec("print(Color.GREEN)");  // 1
  vm->exec("print(Color.BLUE)");   // 2
  vm->exec("print(Color.YELLOW)"); // 3

  delete vm;
}

int add(int x, int y) { return x + y; }

int add(int x, int y, int z) { return x + y + z; }

void register_global(module_ &m) {
  m.def("add", static_cast<int (*)(int, int)>(add));
  m.def("add", static_cast<int (*)(int, int, int)>(add));
}

void test_for_global() {
  std::cout << "-----------------------------------" << std::endl;
  VM *vm = new VM();
  module_ m{vm, vm->_main};
  register_global(m);

  vm->exec("print(add(1, 2))");    // 3
  vm->exec("print(add(1, 2, 3))"); // 6

  delete vm;
}

int main() {
  test_for_point();
  test_for_enum();
  test_for_global();
  return 0;
}