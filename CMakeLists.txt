cmake_minimum_required(VERSION 3.16)

project(compiler)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ir)
add_subdirectory(pass)
add_subdirectory(tests)
