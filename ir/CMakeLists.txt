set(IR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/inst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basicblock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph.cpp
)

add_library(ir SHARED ${IR_SOURCES})
target_include_directories(ir PUBLIC ${PROJECT_SOURCE_DIR})
