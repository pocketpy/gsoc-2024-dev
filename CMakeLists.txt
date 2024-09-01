cmake_minimum_required(VERSION 3.10)

project(gsoc2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(numpy/include)
include_directories(pocketpy/include)

option(SHOW_XTENSOR_WARNINGS "Show warnings from xtensor" OFF)

add_subdirectory(pocketpy)

if(MSVC)
    add_compile_options(/wd4018 /wd4172 /wd4819)
    add_compile_options(/bigobj)
endif()

# Suppress xtensor warnings if SHOW_XTENSOR_WARNINGS is OFF
if(NOT SHOW_XTENSOR_WARNINGS)
    add_compile_definitions(SUPPRESS_XTENSOR_WARNINGS)
    if(MSVC)
        add_compile_options(/wd4244 /wd4267 /wd4018)
    else()
        add_compile_options(-Wno-sign-compare -Wno-conversion -Wno-unused-variable -Wno-unused-parameter)
    endif()
endif()

add_executable(${PROJECT_NAME} numpy/tests/test_numpy.cpp numpy/src/numpy.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY CMAKE_CURRENT_LIST_DIR)

target_link_libraries(${PROJECT_NAME} PRIVATE pocketpy)
