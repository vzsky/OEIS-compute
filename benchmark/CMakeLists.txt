include(FetchContent)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(benchmark)

add_executable(prime_bench PrimeBench.cpp)
target_link_libraries(prime_bench
    PRIVATE benchmark::benchmark prime
)

add_executable(primeint_bench PrimeIntBench.cpp)
target_link_libraries(primeint_bench
    PRIVATE benchmark::benchmark primeint
)

add_executable(bigint_bench BigIntBench.cpp)
target_link_libraries(bigint_bench
    PRIVATE benchmark::benchmark bigint
)

add_executable(a390848_bench A390848Bench.cpp)
target_link_libraries(a390848_bench
    PRIVATE benchmark::benchmark A390848_lib
)
