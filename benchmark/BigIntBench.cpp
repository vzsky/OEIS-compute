#include <benchmark/benchmark.h>
#include <utils/BigInt.h>

using TestInt = DenseBigInt;

static TestInt make_big(int n)
{
  TestInt x(1);
  for (int i = 1; i <= n; ++i) x *= TestInt(i + 1);
  return x;
}

static void BM_add(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n);
  TestInt b = make_big(n / 2);

  for (auto _ : state)
  {
    auto x = a + b;
    benchmark::DoNotOptimize(x);
  }
}

static void BM_sub(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n);
  TestInt b = make_big(n / 2);

  for (auto _ : state)
  {
    auto x = a - b;
    benchmark::DoNotOptimize(x);
  }
}

static void BM_mul(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n / 4);
  TestInt b = make_big(n / 4);

  for (auto _ : state)
  {
    auto x = a * b;
    benchmark::DoNotOptimize(x);
  }
}

static void BM_div(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n);
  TestInt b = make_big(n / 4);

  for (auto _ : state)
  {
    auto x = a / b;
    benchmark::DoNotOptimize(x);
  }
}

static void BM_mod(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n);
  TestInt b = make_big(n / 4);

  for (auto _ : state)
  {
    auto x = a % b;
    benchmark::DoNotOptimize(x);
  }
}

static void BM_cmp(benchmark::State& state)
{
  int n     = state.range(0);
  TestInt a = make_big(n);
  TestInt b = make_big(n);

  for (auto _ : state)
  {
    auto x = (a < b);
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_add)->Range(128, 1024);
BENCHMARK(BM_sub)->Range(128, 1024);
BENCHMARK(BM_mul)->Range(128, 1024);
BENCHMARK(BM_cmp)->Range(128, 1024);
BENCHMARK(BM_div)->Range(128, 1024);
BENCHMARK(BM_mod)->Range(128, 1024);

BENCHMARK_MAIN();
