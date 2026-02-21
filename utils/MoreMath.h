#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <utility>

namespace math
{

template <typename T> T pow(T base, uint64_t k)
{
  T result = 1;
  while (k > 0)
  {
    if (k % 2 == 1) result *= base;
    base *= base;
    k /= 2;
  }
  return result;
}

template <typename T> T pow(T base, uint64_t k, T mod)
{
  T result = 1;
  T b      = base % mod;
  while (k > 0)
  {
    if (k % 2 == 1) result = (result * b) % mod;
    b = (b * b) % mod;
    k /= 2;
  }
  return result;
}

template <typename T> T gcd(T a, T b)
{
  if (a == 0 && b == 0) throw std::invalid_argument("gcd of zeros");
  if (a < 0) a = -a;
  if (b < 0) b = -b;
  if (a < b) std::swap(a, b);

  while (b != 0)
  {
    T r = a % b;
    a   = b;
    b   = r;
  }
  return a;
}

template <typename T> T lcm(T a, T b)
{
  if (a == 0 && b == 0) throw std::invalid_argument("lcm of zeros");
  return a * b / gcd(a, b);
}

template <typename T> T fact(T n)
{
  if (n < 0) throw std::invalid_argument("n must be non-negative");
  T ans = 1;
  for (T i = 1; i <= n; i++) ans *= i;
  return ans;
}

template <typename T> T nCk(T n, T k)
{
  if (k > n) return 0;
  if (k > n - k) k = n - k;
  T res = 1;
  for (T i = 1; i <= k; i++) res *= (n - i + 1);
  for (int i = 1; i <= k; i++) res /= i;
  return res;
}

} // namespace math

namespace stats
{

template <std::ranges::input_range R> double average(R&& r)
{
  size_t n   = std::ranges::distance(r);
  double sum = std::accumulate(std::begin(r), std::end(r), 0.0);
  return sum / n;
}

template <std::ranges::input_range R> double variance(R&& r)
{
  size_t n = std::ranges::distance(r);
  if (n < 2) return 0.0;

  double mean      = average(r);
  auto transformed = r                                                              //
                     | std::views::transform([mean](double x) { return x - mean; }) //
                     | std::views::transform([](double x) { return x * x; });       //

  double sq_sum = std::reduce(std::begin(transformed), std::end(transformed), 0.0, std::plus<>());
  return sq_sum / n;
}

} // namespace stats
