cmake_minimum_required(VERSION 3.16)

project(Compiler)


include(cmake/CPM.cmake)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ROOT_COMPILER ${CMAKE_SOURCE_DIR})
set(COMPILER_SOURCES ${IR_SOURCES}
		     ${PASS_SOURCES}
)
#${CMAKE_SOURCE_DIR}/ir/inst.cpp)


add_subdirectory(ir)
add_subdirectory(pass)
add_subdirectory(tests)
