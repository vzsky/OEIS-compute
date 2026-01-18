#include <benchmark/benchmark.h>
#include <utils/BigInt.h>

template <typename T> static T make_big(int n)
{
  T x(1);
  for (int i = 1; i <= n; ++i)
  {
    x *= i + 1;
    x += i;
  }
  return x;
}

/* ================= ADD ================= */

template <typename T> static void BM_add_impl(benchmark::State& state)
{
  int n = state.range(0);
  T a   = make_big<T>(n);
  T b   = make_big<T>(n / 2);

  for (auto _ : state)
  {
    auto r = a + b;
    benchmark::DoNotOptimize(r);
  }
}

/* ================= SUB ================= */

template <typename T> static void BM_sub_impl(benchmark::State& state)
{
  int n = state.range(0);
  T a   = make_big<T>(n);
  T b   = make_big<T>(n / 2);

  for (auto _ : state)
  {
    auto r = a - b;
    benchmark::DoNotOptimize(r);
  }
}

/* ================= MUL ================= */

template <typename T> static void BM_mul_impl(benchmark::State& state)
{
  int n = state.range(0);
  T a   = make_big<T>(n);
  T b   = make_big<T>(n / 3);

  for (auto _ : state)
  {
    auto r = a * b;
    benchmark::DoNotOptimize(r);
  }
}

/* ================= DIV ================= */

template <typename T> static void BM_div_impl(benchmark::State& state)
{
  int n = state.range(0);
  T a   = make_big<T>(n);
  T b   = make_big<T>(n / 4);

  for (auto _ : state)
  {
    auto r = a / b;
    benchmark::DoNotOptimize(r);
  }
}

/* ================= MOD ================= */

template <typename T> static void BM_mod_impl(benchmark::State& state)
{
  int n = state.range(0);
  T a   = make_big<T>(n);
  T b   = make_big<T>(n / 4);

  for (auto _ : state)
  {
    auto r = a % b;
    benchmark::DoNotOptimize(r);
  }
}

/* ================= REGISTRATION ================= */

static void BM_add_dec(benchmark::State& s) { BM_add_impl<DecBigInt>(s); }
static void BM_add_dense(benchmark::State& s) { BM_add_impl<DenseBigInt>(s); }
static void BM_sub_dec(benchmark::State& s) { BM_sub_impl<DecBigInt>(s); }
static void BM_sub_dense(benchmark::State& s) { BM_sub_impl<DenseBigInt>(s); }
static void BM_mul_dec(benchmark::State& s) { BM_mul_impl<DecBigInt>(s); }
static void BM_mul_dense(benchmark::State& s) { BM_mul_impl<DenseBigInt>(s); }
static void BM_div_dec(benchmark::State& s) { BM_div_impl<DecBigInt>(s); }
static void BM_div_dense(benchmark::State& s) { BM_div_impl<DenseBigInt>(s); }
static void BM_mod_dec(benchmark::State& s) { BM_mod_impl<DecBigInt>(s); }
static void BM_mod_dense(benchmark::State& s) { BM_mod_impl<DenseBigInt>(s); }

BENCHMARK(BM_add_dec)->Name("DecBigInt/Add")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_add_dense)->Name("DenseBigInt/Add")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_sub_dec)->Name("DecBigInt/Sub")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_sub_dense)->Name("DenseBigInt/Sub")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_mul_dec)->Name("DecBigInt/Mul")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_mul_dense)->Name("DenseBigInt/Mul")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_div_dec)->Name("DecBigInt/Div")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_div_dense)->Name("DenseBigInt/Div")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_mod_dec)->Name("DecBigInt/Mod")->Args({128})->Args({512})->Args({2048});
BENCHMARK(BM_mod_dense)->Name("DenseBigInt/Mod")->Args({128})->Args({512})->Args({2048});

BENCHMARK_MAIN();
