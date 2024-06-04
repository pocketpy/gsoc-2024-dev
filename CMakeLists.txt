cmake_minimum_required(VERSION 3.10)

project(gsoc2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(pybind11/include)
include_directories(pocketpy/include)
include_directories(numpy/include)

add_subdirectory(pocketpy)
#add_subdirectory(numpy)

#add_subdirectory(3rd/glm)

add_executable(${PROJECT_NAME} test_main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    #glm
    pocketpy
    #numpy
)

file(GLOB_RECURSE PYBIND11_TEST_SOURCES "pybind11/tests/*.cpp")
add_executable(pybind11_test ${PYBIND11_TEST_SOURCES})

target_link_libraries(pybind11_test PRIVATE
    pocketpy
)
