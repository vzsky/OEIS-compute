#pragma once

#include <cstdint>
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

} // namespace math
