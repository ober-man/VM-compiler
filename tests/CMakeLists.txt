CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)

include_directories(
    ${ROOT_COMPILER}
)

add_executable(
    tests
    ir_test.cpp
    domtree_test.cpp
    rpo_test.cpp
    ${COMPILER_SOURCES}
)

target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/ir")
target_link_libraries(
    tests
    gtest gtest_main
)

target_link_libraries(tests ir pass gtest gtest_main)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR})

add_test(NAME tests COMMAND tests --gtest_color=yes)
