cmake_minimum_required(VERSION 3.14)
project(koster-common-unittests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_library(zephyr-mocks
  ${CMAKE_CURRENT_LIST_DIR}/include/zephyr/kernel.c
  ${CMAKE_CURRENT_LIST_DIR}/include/zephyr/settings/settings.c
  )
target_include_directories(zephyr-mocks PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
  )

enable_testing()
add_subdirectory(default_recipes)
add_subdirectory(parameters)
add_subdirectory(recipe)
