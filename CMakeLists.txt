cmake_minimum_required(VERSION 3.16)

project(compiler)

include(cmake/CPM.cmake)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ROOT_COMPILER ${CMAKE_SOURCE_DIR})
set(COMPILER_SOURCES ${IR_SOURCES}
		     ${PASS_SOURCES}
)

add_subdirectory(ir)
add_subdirectory(pass)
add_subdirectory(tests)

#target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(compiler ir)
#target_link_libraries(compiler pass)
