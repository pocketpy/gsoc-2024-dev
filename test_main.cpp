#include "pocketpy.h"
#include "numpy.hpp"

#include <iostream>
#include <fstream>

using namespace pkpy;

int main(){
    // Create a virtual machine
    VM* vm = new VM();

    // Add numpy module
    add_numpy_module(vm);

    // Load numpy.py
    std::ifstream file("../test_numpy.py");
    if(!file.is_open()){
        std::cerr << "Failed to open test_numpy.py" << std::endl;
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    vm->exec(source);

    // Dispose the virtual machine
    delete vm;
    return 0;
}