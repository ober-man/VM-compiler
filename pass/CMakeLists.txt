set(PASS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/passmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rpo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/domtree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loop_analyzer.cpp
)

add_library(pass SHARED ${PASS_SOURCES})
target_include_directories(pass PUBLIC ${PROJECT_SOURCE_DIR})
