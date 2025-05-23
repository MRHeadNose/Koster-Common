set(TEST_NAME parameters_tests)

# Generated files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

include(code_generator)
generate_code(test-parameters
  ${CMAKE_CURRENT_LIST_DIR}/../../../codegenerators/parameters.py
  ${CMAKE_CURRENT_LIST_DIR}/test_parameters.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include/koster-common/parameters.h
  )

add_executable(${TEST_NAME}
  ${CMAKE_CURRENT_LIST_DIR}/parameters_tests.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${PROJECT_SOURCE_DIR}/../../src/parameters_base.c
)

target_include_directories(${TEST_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include
  ${PROJECT_SOURCE_DIR}/../../include
  ${PROJECT_SOURCE_DIR}/../../src
)

target_link_libraries(${TEST_NAME}
  GTest::gmock_main
  zephyr-mocks
)

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})
