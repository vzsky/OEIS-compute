set(MATH_LIBS
    numbertheory                   NumberTheory.cpp
)

set(ALLMATH "")
list(LENGTH MATH_LIBS _math_len)
math(EXPR _math_len "${_math_len} - 1")

foreach(i RANGE 0 ${_math_len} 2)
  math(EXPR j "${i} + 1")
  list(GET MATH_LIBS ${i} libname)
  list(GET MATH_LIBS ${j} src)
  list(APPEND ALLMATH ${libname})

  add_library(${libname} STATIC ${src})

  target_include_directories(${libname} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
  )
endforeach()

add_library(allmath INTERFACE)
target_link_libraries(allmath INTERFACE ${ALLMATH})

