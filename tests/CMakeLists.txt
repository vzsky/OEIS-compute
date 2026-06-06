set(TEST_MODULES
  metaprog                  testMetaProg.cpp
  prime                     testPrime.cpp
  bigint                    testBigInt.cpp
  modint                    testModInt.cpp
  fraction                  testFraction.cpp
  math                      testMath.cpp
  fft                       testFft.cpp
  treap                     testTreap.cpp
  primeint                  testPrimeInt.cpp
  complementnonnvector      testComplementNonnVector.cpp
  exseq                     testExSeq.cpp
  parallel                  testParallel.cpp
)

enable_testing()

list(LENGTH TEST_MODULES _test_module_len)
math(EXPR _test_module_len "${_test_module_len} - 1")

foreach(i RANGE 0 ${_test_module_len} 2)
  math(EXPR j "${i} + 1")
  list(GET TEST_MODULES ${i} testname)
  list(GET TEST_MODULES ${j} src)

  add_executable(${testname}_test ${src})
  add_test(NAME ${testname}_test COMMAND ${testname}_test --gtest_shuffle)
  target_link_libraries(${testname}_test PRIVATE gtest_main allutils)

endforeach()
