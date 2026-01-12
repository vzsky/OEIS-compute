#include <benchmark/benchmark.h>
#include <utils/Prime.h>

Prime<5000> p;

static void BM_is_prime_until(benchmark::State& state)
{
  for (auto _ : state)
    for (int i = 2; i <= state.range(0); ++i) benchmark::DoNotOptimize(p.is_prime(i));
}

static void BM_factors(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(p.factors(state.range(0)));
}

static void BM_factors_freq(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(p.factors_freq(state.range(0)));
}

static void BM_vector_factors_freq(benchmark::State& state)
{
  for (auto _ : state) benchmark::DoNotOptimize(p.vector_factors_freq(state.range(0)));
}

BENCHMARK(BM_is_prime_until)->Range(1 << 8, 1 << 12)->Arg(1 << 18);

BENCHMARK(BM_factors)->Args({60, 360, 5040, 83160, 1 << 16})->Name("factor - Composite numbers");
BENCHMARK(BM_factors_freq)->Args({60, 360, 5040, 83160, 1 << 16})->Name("factor_freq - Composite numbers");
BENCHMARK(BM_vector_factors_freq)
    ->Args({60, 360, 5040, 83160, 1 << 16})
    ->Name("vector_factors_freq - Composite numbers");

BENCHMARK(BM_factors)
    ->Args({2347, 4567, 7919, 99991, 104729, 918234121, (1llu << 20) * 7919 * 7919})
    ->Name("factors - Primes");
BENCHMARK(BM_factors_freq)
    ->Args({2347, 4567, 7919, 99991, 104729, 918234121, (1llu << 20) * 7919 * 7919})
    ->Name("factors_freq - Primes");
BENCHMARK(BM_vector_factors_freq)
    ->Args({2347, 4567, 7919, 99991, 104729, 918234121, (1llu << 20) * 7919 * 7919})
    ->Name("vector_factors_freq - Primes");

BENCHMARK_MAIN();
