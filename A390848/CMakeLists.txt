set(DEPENDENCIES
  allutils
)

get_filename_component(EXEC_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_executable(${EXEC_NAME} main.cpp)
target_link_libraries(${EXEC_NAME} PRIVATE ${DEPENDENCIES})

