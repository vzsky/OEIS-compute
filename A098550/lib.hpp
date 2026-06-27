#pragma once

#include <cstdint>
#include <vector>

template <uint64_t PrimeBound> struct LazyFactors
{
  // contains all factors that are proven to be prime.
  // all prime factors <= PrimeBound are in, but a element can be >= PrimeBound
  std::vector<uint64_t> primes;
  // so remainder = product of factor > PrimeBound, thus remainder >= PrimeBound^2
  uint64_t remainder = 1;

  [[nodiscard]] std::vector<uint64_t> remaining_distinct_factors() const
  {
    std::vector<uint64_t> factors;
    uint64_t n = remainder;
    for (uint64_t d = PrimeBound + 1 + (PrimeBound % 2); d * d <= n; d += 2)
    {
      if (n % d == 0)
      {
        factors.push_back(d);
        while (n % d == 0) n /= d;
      }
    }
    if (n > 1) factors.push_back(n);
    return factors;
  }
};

template <uint64_t PrimeBound> struct LazyPrimeSieve
{
  LazyPrimeSieve()
  {
    std::vector<bool> sieve(PrimeBound + 1, true);
    sieve[0] = sieve[1] = false;
    for (uint64_t i = 2; i * i <= PrimeBound; ++i)
      if (sieve[i])
        for (uint64_t j = i * i; j <= PrimeBound; j += i) sieve[j] = false;
    for (uint64_t i = 2; i <= PrimeBound; ++i)
      if (sieve[i]) mPrimes.push_back(i);
  }

  [[nodiscard]] LazyFactors<PrimeBound> distinct_factors(uint64_t n) const
  {
    LazyFactors<PrimeBound> result;
    for (uint64_t p : mPrimes)
    {
      if (p * p > n) break;
      if (n % p == 0)
      {
        result.primes.push_back(p);
        while (n % p == 0) n /= p;
      }
    }
    if (n > 1)
    {
      if (n < PrimeBound * PrimeBound)
        result.primes.push_back(n);
      else
        result.remainder = n;
    }
    return result;
  }

private:
  std::vector<uint64_t> mPrimes;
};
