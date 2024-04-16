set(PASS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/passmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rpo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/domtree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loop_analysis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/const_folding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/checks_elimination.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/peepholes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linear_order.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/liveness.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_alloc.cpp
)

add_library(pass SHARED ${PASS_SOURCES})
target_include_directories(pass PUBLIC ${PROJECT_SOURCE_DIR})
