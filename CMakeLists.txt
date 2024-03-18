cmake_minimum_required(VERSION 3.10)

project(test)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rd/glm)
add_subdirectory(3rd/pybind11)

add_executable(${PROJECT_NAME} pkpy_with_pybind11.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
