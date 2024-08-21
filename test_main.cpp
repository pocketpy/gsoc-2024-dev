#include <fstream>
#include <pybind11/embed.h>
#include <sstream>
#include <string>

namespace py = pybind11;
using namespace pybind11;

#include "pocketpy.h"

static char buf[2048];

int main() {
  py::scoped_interpreter guard{};

  while(true) {
      int size = py_replinput(buf, sizeof(buf));
      assert(size < sizeof(buf));
      if(size >= 0) {
          py_StackRef p0 = py_peek(0);
          if(!py_exec(buf, "<stdin>", SINGLE_MODE, NULL)) {
              py_printexc();
              py_clearexc(p0);
          }
      }
  }

  return 0;
}