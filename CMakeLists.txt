cmake_minimum_required(VERSION 3.10)

project(gsoc2024)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -g -fno-sanitize=function")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -g -fno-sanitize=function")

include_directories(numpy/include)
include_directories(pybind11/include)
include_directories(pocketpy/include)

add_subdirectory(pocketpy)

if(MSVC)
    add_compile_options(/wd4018 /wd4172 /wd4819)
    add_compile_options(/bigobj)
endif()

add_executable(${PROJECT_NAME} test_main.cpp numpy/src/numpy.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY CMAKE_CURRENT_LIST_DIR)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pocketpy
)

if(ENABLE_TEST)
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    # git clone https://github.com/google/googletest.git
    include_directories(googletest/googletest/include)
    add_subdirectory(googletest)

    file(GLOB_RECURSE PYBIND11_TEST_SOURCES "pybind11/tests/*.cpp")
    add_executable(pybind11_test ${PYBIND11_TEST_SOURCES})

    target_link_libraries(pybind11_test PRIVATE
        pocketpy
        GTest::gtest_main
    )
endif()
