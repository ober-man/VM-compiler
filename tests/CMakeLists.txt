find_package(GTest REQUIRED)

set(TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rpo_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/domtree_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loop_analyzer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/const_folding_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linear_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/liveness_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_alloc_test.cpp
)

add_executable(tests ${TESTS_SOURCES})
target_link_libraries(tests ir pass gtest gtest_main pthread)
target_include_directories(tests
	PRIVATE ${PROJECT_SOURCE_DIR}/ir
	PRIVATE ${PROJECT_SOURCE_DIR}/pass
)

add_test(NAME tests COMMAND tests --gtest_color=yes)
