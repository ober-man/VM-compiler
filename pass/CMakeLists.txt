set(PASS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rpo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/domtree.cpp
)

add_library(pass SHARED ${PASS_SOURCES})
target_include_directories(pass PRIVATE ${PROJECT_SOURCE_DIR})
