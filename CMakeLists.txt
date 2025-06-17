zephyr_library()

# Generated files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(code_generator)
generate_code(koster-common-parameters
  ${CMAKE_CURRENT_LIST_DIR}/codegenerators/parameters.py
  ${CMAKE_CURRENT_LIST_DIR}/config/parameters.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include/koster-common/parameters.h
  )

generate_code(koster-common-default-recipes
  ${CMAKE_CURRENT_LIST_DIR}/codegenerators/default_recipes.py
  ${CMAKE_CURRENT_LIST_DIR}/config/default_recipes.xml
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.h
  )

zephyr_library_sources(
  ${CMAKE_CURRENT_LIST_DIR}/src/koster-common.c
  ${CMAKE_CURRENT_LIST_DIR}/src/koster-settings.c
  ${CMAKE_CURRENT_LIST_DIR}/src/koster-zbus.c
  ${CMAKE_CURRENT_LIST_DIR}/src/parameters_base.c
  ${CMAKE_CURRENT_LIST_DIR}/src/program_logger.c
  ${CMAKE_CURRENT_LIST_DIR}/src/recipe.c
  ${CMAKE_CURRENT_LIST_DIR}/src/default_recipes.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/parameters.c
  ${CMAKE_CURRENT_BINARY_DIR}/generated/default_recipes_generated.c
  )

# Public include directory
zephyr_include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/include # public includes
  ${CMAKE_CURRENT_BINARY_DIR}/generated/include # public generated includes
  )

# Private include directories
zephyr_library_include_directories(PRIVATE
  ${ZEPHYR_BASE}/../modules/fs/littlefs
  ${ZEPHYR_BASE}/subsys/fs
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/generated # private generated includes
  )
