set(TEST_NAME default_recipes_tests)

# Generated files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

include(code_generator)
generate_code(test-default-recipes
  ${CMAKE_CURRENT_LIST_DIR}/../../../codegenerators/default_recipes.py
  ${CMAKE_CURRENT_LIST_DIR}/test_default_recipes.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.h
  )
generate_code(test-default-recipes-parameters
  ${CMAKE_CURRENT_LIST_DIR}/../../../codegenerators/parameters.py
  ${CMAKE_CURRENT_LIST_DIR}/test_default_recipes_parameters.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/koster-common/parameters.h
  )

add_executable(${TEST_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/default_recipes_tests.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${PROJECT_SOURCE_DIR}/../../src/default_recipes.c
  ${PROJECT_SOURCE_DIR}/../../src/parameters_base.c
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
