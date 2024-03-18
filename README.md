# gsoc-2024-dev

# Introduction

This is the repository for GSoC'24 projects. [Implement pybind11 for bindings](https://pocketpy.dev/gsoc/ideas/#implement-pybind11-for-bindings) and [Add numpy module](https://pocketpy.dev/gsoc/ideas/#add-numpy-module).

# File structure

### For [@16bit-ykiko](https://github.com/16bit-ykiko)
+ Use `pybind11/` directory for development
+ Use `test_pybind11.cpp` for testing (without numpy)
+ Use `test_main.cpp` for testing (with numpy & CMake)

### For [@faze-geek](https://github.com/faze-geek)
+ Use `numpy/` directory for development
+ Use `test_numpy.py` for testing, numpy module adopts only pure python test
+ Use `test_main.cpp` for testing. This file invokes `test_numpy.py`

If you need to alter files which are not owned by you, please ask your mentor first.

# Build and test on Linux

```sh
mkdir build
cd build
cmake ..
cmake --build .
./gsoc2024
```
