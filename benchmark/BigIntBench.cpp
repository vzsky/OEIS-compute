#include <benchmark/benchmark.h>
#include <utils/BigInt.h>

using TestInt = DecBigInt;

static TestInt make_big(int n)
{
  TestInt x(1);
  for (int i = 1; i <= n; ++i) x = x * TestInt(i + 1);
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

static void BM_copy_ctor(benchmark::State& state)
{
  int n = state.range(0);

  for (auto _ : state)
  {
    TestInt a = make_big(n);
    TestInt x(a);
    benchmark::DoNotOptimize(a);
    benchmark::DoNotOptimize(x);
  }
}

static void BM_move_ctor(benchmark::State& state)
{
  int n = state.range(0);

  for (auto _ : state)
  {
    TestInt a = make_big(n);
    TestInt x(std::move(a));
    benchmark::DoNotOptimize(x);
  }
}

BENCHMARK(BM_add)->Range(10, 1000);
BENCHMARK(BM_sub)->Range(10, 1000);
BENCHMARK(BM_mul)->Range(10, 1000);
BENCHMARK(BM_cmp)->Range(10, 1000);

BENCHMARK(BM_copy_ctor)->Range(10, 2000);
BENCHMARK(BM_move_ctor)->Range(10, 2000);

BENCHMARK_MAIN();
