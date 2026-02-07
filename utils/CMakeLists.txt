set(UTILS_LIBS
    utils                   Utils.cpp
    treap                   Treap.cpp
    bigint                  BigInt.cpp
    modint                  ModInt.cpp
    fraction                Fraction.cpp
    primeint                PrimeInt.cpp
    prime                   Prime.cpp
    moremath                MoreMath.cpp
    complementnonnvector    ComplementNonnVector.cpp
)

set(ALLUTILS "")
list(LENGTH UTILS_LIBS _utils_len)
math(EXPR _utils_len "${_utils_len} - 1")

foreach(i RANGE 0 ${_utils_len} 2)
  math(EXPR j "${i} + 1")
  list(GET UTILS_LIBS ${i} libname)
  list(GET UTILS_LIBS ${j} src)
  list(APPEND ALLUTILS ${libname})

  add_library(${libname} STATIC ${src})

  target_include_directories(${libname} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
  )
endforeach()

target_include_directories(bigint
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GMP_INCLUDE_DIR}
)

target_link_libraries(bigint PUBLIC 
    ${GMPXX_LIB} 
    ${GMP_LIB} 
    pthread
)

target_link_libraries(fraction PUBLIC bigint moremath)
target_link_libraries(primeint PUBLIC prime)

add_library(allutils INTERFACE)
target_link_libraries(allutils INTERFACE ${ALLUTILS})

