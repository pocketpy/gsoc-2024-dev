cmake_minimum_required(VERSION 3.12)
project(numpy_bindings)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include pybind11
add_subdirectory(include/pybind11)

# Define your module
pybind11_add_module(numpy_bindings src/numpy.cpp)

# Include directories for numpy headers
target_include_directories(numpy_bindings PRIVATE include)

# Link any additional libraries if needed
# target_link_libraries(numpy_bindings PRIVATE ...)
