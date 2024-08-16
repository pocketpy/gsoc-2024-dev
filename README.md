# gsoc-2024-dev

# Introduction

This is the repository for GSoC'24 projects. [Implement pybind11 for bindings](https://pocketpy.dev/gsoc/ideas/#implement-pybind11-for-bindings) and [Add numpy module](https://pocketpy.dev/gsoc/ideas/#add-numpy-module).

# File structure

### For the pybind11 project
+ Use `pybind11/` directory for development
+ Use `test_pybind11.cpp` for testing (without numpy)
+ Use `test_main.cpp` for testing (with numpy & CMake)

### For the numpy project
+ Use `numpy/` directory for development
+ Use `test_numpy.py` for testing, numpy module adopts only pure python test
+ Use `test_main.cpp` for testing. This file invokes `test_numpy.py`

If you need to alter files which are not owned by you, please ask your mentor first.

# Setup Instructions

Clone the repository:
```sh
git clone https://github.com/pocketpy/gsoc-2024-dev.git
```

Initialize and update the pocketpy sub-module with this command:
```sh
cd gsoc-2024-dev
git submodule update --init --recursive
```

# Build and test on Linux

```sh
cmake -B build
cmake --build build
build/gsoc2024
```
