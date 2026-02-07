#include <benchmark/benchmark.h>
#include <unordered_map>
#include <utils/BigInt.h>

using slow_bigint::DenseBigInt;

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
DEFINE_BENCH(add, DenseBigInt);
DEFINE_BENCH(add, BigInt);

RUN_BENCH(add, DenseBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(add, BigInt)->Args({128})->Args({512})->Args({8192});

// --- SUB ---
DEFINE_BENCH(sub, DenseBigInt);
DEFINE_BENCH(sub, BigInt);

RUN_BENCH(sub, DenseBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(sub, BigInt)->Args({128})->Args({512})->Args({8192});

// --- MUL ---
DEFINE_BENCH(mul, DenseBigInt);
DEFINE_BENCH(mul, BigInt);

RUN_BENCH(mul, DenseBigInt)->Args({128})->Args({512})->Args({8192})->Args({32768});
RUN_BENCH(mul, BigInt)->Args({128})->Args({512})->Args({8192})->Args({32768});

// --- DIV ---

DEFINE_BENCH(div, DenseBigInt);
DEFINE_BENCH(div, BigInt);

RUN_BENCH(div, DenseBigInt)->Args({128})->Args({512})->Args({8192});
RUN_BENCH(div, BigInt)->Args({128})->Args({512})->Args({8192});

BENCHMARK_MAIN();
