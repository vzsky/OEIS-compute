#include <benchmark/benchmark.h>
#include <unordered_map>
#include <utils/BigInt.h>

template <typename T> static T make_big(int n)
{
  static std::unordered_map<int, T> cache;
  auto it = cache.find(n);
  if (it != cache.end()) return it->second;

  T x(1);
  for (int i = 1; i <= n; ++i)
  {
    x *= i + 1;
    x += i;
  }
  return cache[n] = x;
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

/* ================= REGISTRATION ================= */

#define DEFINE_BENCH(OP, TYPE)                                                                               \
  static void BM_##OP##_##TYPE(benchmark::State& s) { BM_##OP##_impl<TYPE>(s); }

#define RUN_BENCH(OP, TYPE) BENCHMARK(BM_##OP##_##TYPE)->Name(#TYPE "/" #OP)

// --- ADD ---
DEFINE_BENCH(add, DecBigInt);
DEFINE_BENCH(add, DenseDecBigInt);
DEFINE_BENCH(add, DenseBigInt);

RUN_BENCH(add, DecBigInt)->Args({128});
RUN_BENCH(add, DenseDecBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(add, DenseBigInt)->Args({128})->Args({512})->Args({8192});

// --- SUB ---
DEFINE_BENCH(sub, DecBigInt);
DEFINE_BENCH(sub, DenseDecBigInt);
DEFINE_BENCH(sub, DenseBigInt);

RUN_BENCH(sub, DecBigInt)->Args({128});
RUN_BENCH(sub, DenseDecBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(sub, DenseBigInt)->Args({128})->Args({512})->Args({8192});

// --- MUL ---
DEFINE_BENCH(mul, DecBigInt);
DEFINE_BENCH(mul, DenseDecBigInt);
DEFINE_BENCH(mul, DenseBigInt);

RUN_BENCH(mul, DecBigInt)->Args({128});
RUN_BENCH(mul, DenseDecBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(mul, DenseBigInt)->Args({128})->Args({512})->Args({8192})->Args({32768});

// --- DIV ---

DEFINE_BENCH(div, DecBigInt);
DEFINE_BENCH(div, DenseDecBigInt);
DEFINE_BENCH(div, DenseBigInt);

RUN_BENCH(div, DecBigInt)->Args({128});
RUN_BENCH(div, DenseDecBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(div, DenseBigInt)->Args({128})->Args({512})->Args({8192});

BENCHMARK_MAIN();
