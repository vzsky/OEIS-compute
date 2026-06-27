#pragma once

#include <cstdint>
#include <math/Basic.hpp>

namespace maya {

[[nodiscard]] constexpr bool is_prime(uint32_t n)
{
  if (n < 2) return false;
  if (n == 2) return true;
  if (n % 2 == 0) return false;
  for (uint64_t i = 3; i * i <= n; i += 2)
    if (n % i == 0) return false;
  return true;
}

[[nodiscard]] constexpr bool is_coprime(uint32_t a, uint32_t b) { return math::gcd(a, b) == 1; }

[[nodiscard]] constexpr size_t prime_count(uint64_t n)
{
  size_t count = 0;
  for (uint64_t i = 2; i <= n; ++i)
    if (is_prime(static_cast<uint32_t>(i))) count++;
  return count;
};

template <size_t n> [[nodiscard]] constexpr std::array<uint64_t, prime_count(n)> primes_upto()
{
  std::array<uint64_t, prime_count(n)> primes{};
  int idx = 0;
  for (uint64_t i = 2; i <= n; ++i)
  {
    if (is_prime(i)) primes[idx++] = i;
  }
  return primes;
}

} // namespace maya
