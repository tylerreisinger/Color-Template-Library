cmake_minimum_required(VERSION 2.6)
project(cppcolor)


add_subdirectory(${PROJECT_SOURCE_DIR}/src)
include_directories("src")

list(APPEND CMAKE_CXX_FLAGS "-std=c++14")
#list(APPEND CMAKE_CXX_FLAGS "-std=c++14 -fprofile-arcs -ftest-coverage")

set(PROJECT_BIN_DIR "${CMAKE_BINARY_DIR}/bin")

if(DEFINED BUILD_TEST_PROGRAMS)
    message("Building test binaries...")
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()
