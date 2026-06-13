set(STATIC_LIBS
    complementnonnvector    ComplementNonnVector.cpp
    primeint                PrimeInt.cpp
)

set(INTERFACE_LIBS
    bigint
    fft
    fraction
    modint
    prime
    treap
    utils
)

set(ALLUTILS "")

list(LENGTH STATIC_LIBS _static_len)
math(EXPR _static_len "${_static_len} - 1")
foreach(i RANGE 0 ${_static_len} 2)
  math(EXPR j "${i} + 1")
  list(GET STATIC_LIBS ${i} libname)
  list(GET STATIC_LIBS ${j} src)
  list(APPEND ALLUTILS ${libname})
  add_library(${libname} STATIC ${src})
  target_include_directories(${libname} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}
  )
endforeach()

foreach(libname IN LISTS INTERFACE_LIBS)
  list(APPEND ALLUTILS ${libname})
  add_library(${libname} INTERFACE)
  target_include_directories(${libname} INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}
  )
endforeach()

target_include_directories(bigint INTERFACE ${GMP_INCLUDE_DIR})
target_link_libraries(bigint INTERFACE ${GMPXX_LIB} ${GMP_LIB} pthread)
target_link_libraries(fraction INTERFACE bigint)
target_link_libraries(primeint PUBLIC prime)

add_library(allutils INTERFACE)
target_link_libraries(allutils INTERFACE ${ALLUTILS})
