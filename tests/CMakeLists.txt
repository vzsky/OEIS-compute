set(UTILS_TESTS
  testPrime.cpp
  testBigInt.cpp
  testModInt.cpp
  testMoreMath.cpp
  testTreap.cpp
)

enable_testing()

add_executable(utils_test ${UTILS_TESTS})
target_link_libraries(utils_test PRIVATE
  gtest_main
  utils 
  prime 
  bigint 
  modint 
  math
  treap
)


add_executable(exseq_test testExSeq.cpp)
target_link_libraries(exseq_test PRIVATE
  gtest_main
  A002326_lib
)
