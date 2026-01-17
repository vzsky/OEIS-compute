set(UTILS_LIBS
    utils                   Utils.cpp
    treap                   Treap.cpp
    bigint                  BigInt.cpp
    modint                  ModInt.cpp
    Fraction                Fraction.cpp
    primeint                PrimeInt.cpp
    prime                   Prime.cpp
    moremath                MoreMath.cpp
    complementnonnvector    ComplementNonnVector.cpp
)

set(ALLUTILS_SOURCES "")
list(LENGTH UTILS_LIBS _utils_len)
math(EXPR _utils_len "${_utils_len} - 1")

foreach(i RANGE 0 ${_utils_len} 2)
  math(EXPR j "${i} + 1")
  list(GET UTILS_LIBS ${i} libname)
  list(GET UTILS_LIBS ${j} src)
  list(APPEND ALLUTILS_SOURCES ${src})

  add_library(${libname} STATIC ${src})

  target_include_directories(${libname} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
  )
endforeach()

add_library(allutils STATIC ${ALLUTILS_SOURCES})
target_include_directories(allutils PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}
)
