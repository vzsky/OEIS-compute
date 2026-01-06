set(UTILS_LIBS
    treap    Treap.cpp
    bigint   BigInt.cpp
    modint   ModInt.cpp
    primeint PrimeInt.cpp
    prime    Prime.cpp
    math     MoreMath.cpp
)

# -------------------------------------------
# Libraries
# -------------------------------------------

add_library(utils STATIC Utils.cpp)

target_include_directories(utils PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}
      ${PROJECT_SOURCE_DIR}
)

list(LENGTH UTILS_LIBS _utils_len)
math(EXPR _utils_len "${_utils_len} - 1")

foreach(i RANGE 0 ${_utils_len} 2)

  math(EXPR j "${i} + 1")

  list(GET UTILS_LIBS ${i} libname)
  list(GET UTILS_LIBS ${j} src)

  add_library(${libname} STATIC ${src})

  target_include_directories(${libname} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}
  )
endforeach()
