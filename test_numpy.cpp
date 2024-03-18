#include "pocketpy.h"
#include "numpy.hpp"

using namespace pkpy;

int main(){
    // Create a virtual machine
    VM* vm = new VM();

    // Add numpy module
    add_numpy_module(vm);

    // Hello world!
    vm->exec("import numpy as np");
    vm->exec("np.hello()");

    // Dispose the virtual machine
    delete vm;
    return 0;
}