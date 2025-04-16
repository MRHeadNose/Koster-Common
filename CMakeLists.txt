zephyr_library()

zephyr_library_sources(
  ${CMAKE_CURRENT_LIST_DIR}/src/recipe.c
  ${CMAKE_CURRENT_LIST_DIR}/src/koster-zbus.c
  )

# Public include directory
zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# Private include directories
zephyr_library_include_directories(PRIVATE ${ZEPHYR_BASE}/../modules/fs/littlefs)
zephyr_library_include_directories(PRIVATE ${ZEPHYR_BASE}/subsys/fs)
