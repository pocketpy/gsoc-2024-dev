# pybind11 for pocketpy

pocketpy 提供 pybind11 的兼容性绑定接口。它使用 C++17 编写，拥有更快的编译速度。并且针对一些情况做了额外的优化（见后文），效率和您手写的绑定代码相当。


## Usage

```cpp
#include <pybind11/pybind11.h>

int main(){
    pybind11::init();
    pybind11::print("Hello, World!");
    return 0;
}
```

## Features








