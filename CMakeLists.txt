cmake_minimum_required(VERSION 3.10)

project(gsoc2024)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rd/glm)

add_subdirectory(pocketpy)
add_subdirectory(numpy)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glm
    pocketpy
    numpy
)
