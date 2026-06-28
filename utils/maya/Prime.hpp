#pragma once

#include <array>
#include <cstdint>
#include <math/Basic.hpp>

namespace maya {

[[nodiscard]] constexpr bool is_prime(uint64_t n)
{
  if (n < 2) return false;
  if (n == 2 || n == 3) return true;
  if (n % 2 == 0 || n % 3 == 0) return false;
  for (uint64_t i = 5; i * i <= n; i += 6)
    if (n % i == 0 || n % (i + 2) == 0) return false;
  return true;
}

[[nodiscard]] constexpr bool is_coprime(uint64_t a, uint64_t b) { return math::gcd(a, b) == 1; }

[[nodiscard]] constexpr size_t prime_count(uint64_t n)
{
  size_t count = 0;
  for (uint64_t i = 2; i <= n; ++i)
    if (is_prime(i)) count++;
  return count;
}

template <size_t N> [[nodiscard]] consteval std::array<uint64_t, prime_count(N)> primes_upto()
{
  std::array<uint64_t, prime_count(N)> primes{};
  size_t idx = 0;
  for (size_t i = 2; i <= N; ++i)
    if (is_prime(i)) primes[idx++] = i;
  return primes;
}

} // namespace maya
