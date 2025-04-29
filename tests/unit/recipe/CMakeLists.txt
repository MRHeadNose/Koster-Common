set(TEST_NAME recipe_tests)

add_executable(${TEST_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/recipe_tests.cpp
  ${PROJECT_SOURCE_DIR}/../../src/recipe.c
)

target_include_directories(${TEST_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/../../include
  ${PROJECT_SOURCE_DIR}/../../src
)

target_link_libraries(${TEST_NAME}
  GTest::gmock_main
  zephyr-mocks
)

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})
