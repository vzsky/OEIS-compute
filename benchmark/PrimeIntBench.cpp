#include <benchmark/benchmark.h>
#include <cstdint>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <vector>

Prime<10000> p{};

PrimeInt pi(int64_t n) { return p.vector_factors_freq(n); }

std::vector<PrimeInt> numbers = []
{
  std::vector<PrimeInt> n;
  for (int64_t base : {5040, 3600, 919, 243})
    for (int mult = 1; mult <= 512; mult *= 2) n.push_back(pi(base) * pi(mult));
  return n;
}();
const size_t n_all_pairs = numbers.size() * numbers.size();

static const std::vector<std::pair<size_t, size_t>> divisible_pairs = []
{
  std::vector<std::pair<size_t, size_t>> pairs;
  for (size_t i = 0; i < numbers.size(); ++i)
    for (size_t j = 0; j < numbers.size(); ++j)
      if (numbers[i].is_divisible_by(numbers[j])) pairs.emplace_back(i, j);
  return pairs;
}();

// --------------------
// Multiplication benchmark
// --------------------
static void BM_multiply(benchmark::State& state)
{
  for (auto _ : state)
    for (size_t i = 0; i < numbers.size(); ++i)
      for (size_t j = 0; j < numbers.size(); ++j)
        benchmark::DoNotOptimize(numbers[i] * numbers[j]);
}
BENCHMARK(BM_multiply)
    ->Name(("Multiply - " + std::to_string(n_all_pairs) + " pairs").c_str());

// --------------------
// Division benchmark
// --------------------
static void BM_divide(benchmark::State& state)
{
  for (auto _ : state)
    for (auto [i, j] : divisible_pairs)
      benchmark::DoNotOptimize(numbers[i] / numbers[j]);
}
BENCHMARK(BM_divide)->Name(
    ("Divide - " + std::to_string(divisible_pairs.size()) + " pairs").c_str());

// --------------------
// Divisibility benchmark
// --------------------
static void BM_is_divisible(benchmark::State& state)
{
  for (auto _ : state)
    for (size_t i = 0; i < numbers.size(); ++i)
      for (size_t j = 0; j < numbers.size(); ++j)
        benchmark::DoNotOptimize(numbers[i].is_divisible_by(numbers[j]));
}
BENCHMARK(BM_is_divisible)
    ->Name(("isDivisible - " + std::to_string(n_all_pairs) + " pairs").c_str());

BENCHMARK_MAIN();
