set(TEST_NAME recipe_tests)

# Generated files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

include(generate_parameters)
generate_parameters(test-recipes
  ${CMAKE_CURRENT_LIST_DIR}/../../../codegenerators/default_recipes.py
  ${CMAKE_CURRENT_LIST_DIR}/test_default_recipes.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.h
  )

add_executable(${TEST_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/recipe_tests.cpp
  ${PROJECT_SOURCE_DIR}/../../src/recipe.c
)

target_include_directories(${TEST_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/../../include
  ${PROJECT_SOURCE_DIR}/../../src
  ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(${TEST_NAME}
  GTest::gmock_main
  zephyr-mocks
)

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})
