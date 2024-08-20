#include <fstream>
#include <pybind11/embed.h>
#include <sstream>
#include <string>

namespace py = pybind11;
using namespace pybind11;

int main() {
  py::scoped_interpreter guard{};
  std::ifstream file("test_numpy.py");
  if(!file.is_open()){
    std::cerr << "Could not open file" << std::endl;
    return 1;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string script = buffer.str();

  try{
    py::exec(script);
  }catch(py::error_already_set& e){
    std::cerr << e.what() << std::endl;
  }

  return 0;
}