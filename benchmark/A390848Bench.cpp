#include <A389544/lib.h>
#include <A390848/lib.h>
#include <benchmark/benchmark.h>

constexpr uint32_t CacheLim = 2e9;
template <uint32_t N> struct A390848 : public A389544<N, CacheLim>
{
};

static void BM_until_10k(benchmark::State& state)
{
  for (auto _ : state)
  {
    A390848<10'000> e{};
    e.get_sequence_until_N();
    benchmark::DoNotOptimize(e);
  }
}

BENCHMARK(BM_until_10k);

BENCHMARK_MAIN();
