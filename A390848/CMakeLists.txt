set(DEPENDENCIES
  utils
  bigint
  primeint
  modint
  prime
)

set(FILES
  main.cpp
)

get_filename_component(EXEC_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_library(${EXEC_NAME}_lib STATIC ${FILES})
add_executable(${EXEC_NAME} ${FILES})
target_link_libraries(${EXEC_NAME}_lib PUBLIC ${DEPENDENCIES})
target_link_libraries(${EXEC_NAME} PRIVATE ${EXEC_NAME}_lib)
