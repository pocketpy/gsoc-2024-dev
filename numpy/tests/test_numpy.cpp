#include <pybind11/embed.h>
#include <fstream>
#include <sstream>
#include <string>
  
namespace py = pybind11;
using namespace pybind11;
  
int main() {
  py::scoped_interpreter guard{};
  std::ifstream file("numpy/tests/test_numpy.py");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string script = buffer.str();
  py::exec(script);

  return 0;
}
