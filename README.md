# gsoc-2024-dev

# Build and test on Linux

```sh
mkdir build
cd build
cmake ..
cmake --build .
./gsoc2024
```

# File structure

## For @16bit-ykiko
+ Use `pybind11/` directory for development
+ Use `test_pybind11.cpp` for testing (without numpy)
+ Use `test_main.cpp` for testing (with numpy & CMake)

## For @faze-geek
+ Use `numpy/` directory for development
+ Use `test_numpy.py` for testing, numpy module adopts only pure python test
+ Use `test_main.cpp` for testing. This file invokes `test_numpy.py`

If you need to alter files which are not owned by you, please ask your mentor first.
