zephyr_library()

# Generated files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(generate_parameters)
generate_parameters(koster-common-parameters
  ${CMAKE_CURRENT_LIST_DIR}/codegenerators/parameters.py
  ${CMAKE_CURRENT_LIST_DIR}/config/parameters.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include/koster-common/parameters.h
  )

zephyr_library_sources(
  ${CMAKE_CURRENT_LIST_DIR}/src/recipe.c
  ${CMAKE_CURRENT_LIST_DIR}/src/koster-zbus.c
  ${CMAKE_CURRENT_LIST_DIR}/src/parameters_base.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  )

# Public include directory
zephyr_include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include
  )

# Private include directories
zephyr_library_include_directories(PRIVATE
  ${ZEPHYR_BASE}/../modules/fs/littlefs
  ${ZEPHYR_BASE}/subsys/fs
  ${CMAKE_CURRENT_LIST_DIR}/src
  )
