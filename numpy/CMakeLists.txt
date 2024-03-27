cmake_minimum_required(VERSION 3.10)

project(numpy)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../pocketpy/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../pybind11/include)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC)

add_library(${PROJECT_NAME} STATIC ${SRC})
