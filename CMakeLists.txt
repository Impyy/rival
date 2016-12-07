cmake_minimum_required(VERSION 2.8)
project(rival)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-D_GNU_SOURCE)

find_package(PkgConfig REQUIRED)

add_subdirectory(src)
