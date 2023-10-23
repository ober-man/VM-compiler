cmake_minimum_required(VERSION 3.16)

project(compiler)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ir)
add_subdirectory(pass)
add_subdirectory(tests)

#add_executable(compiler compiler.cpp)

#target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(compiler ir)
#target_link_libraries(compiler pass)
