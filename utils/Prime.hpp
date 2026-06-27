#pragma once

#include <map>
#include <math/Basic.hpp>
#include <stdexcept>
#include <utils/Logging.hpp>
#include <vector>

template <uint64_t N> class PrimeSieve
{
  static_assert(N > 2);

public:
  using prime_t = uint64_t;
  using exp_t   = uint64_t;

  PrimeSieve();

  [[nodiscard]] bool is_prime(uint64_t n) const;

  [[nodiscard]] std::vector<prime_t> distinct_factors(uint64_t n) const
  {
    std::vector<prime_t> result;
    emit_factors(n, [&](prime_t p)
    {
      if (result.empty() || p != result.back()) result.push_back(p);
    });
    return result;
  }

  [[nodiscard]] std::vector<prime_t> factors(uint64_t n) const
  {
    std::vector<prime_t> result;
    emit_factors(n, [&](prime_t p) { result.push_back(p); });
    return result;
  }

  [[nodiscard]] std::map<prime_t, exp_t> factors_freq(uint64_t n) const
  {
    std::map<prime_t, exp_t> freq;
    emit_factors(n, [&](prime_t p) { ++freq[p]; });
    return freq;
  }

  // I hate this API. std::pair<int,int> + getable from map
  [[nodiscard]] std::vector<std::pair<prime_t, exp_t>> vector_factors_freq(uint64_t n) const
  {
    std::vector<std::pair<prime_t, exp_t>> freq;
    emit_factors(n, [&](prime_t p)
    {
      if (freq.empty() || freq.back().first != p)
        freq.emplace_back(p, 1);
      else
        ++freq.back().second;
    });
    return freq;
  }

  [[nodiscard]] const std::vector<prime_t>& all_primes() const { return mAllPrimes; };
  [[nodiscard]] prime_t lowest_prime_factor(uint64_t n) const { return mLowestPrimeDiv[n]; }
  [[nodiscard]] prime_t highest_prime_factor(uint64_t n) const
  {
    uint64_t result;
    emit_factors(n, [&](prime_t p) { result = p; });
    return result;
  }

private:
  // callback will be called in sorted order (small to large primes)
  void emit_factors(uint64_t /*n*/, auto /*callback*/) const;

  std::vector<prime_t> mLowestPrimeDiv = std::vector<prime_t>(N + 1); // lowest prime divisor of i
  std::vector<prime_t> mAllPrimes;
};

template <uint64_t N> PrimeSieve<N>::PrimeSieve()
{
  Log(LL::Infra, "constructing a prime helper with N=$", N);

  mAllPrimes.reserve(N / std::log(N));

  for (uint64_t i = 0; i <= N; ++i) mLowestPrimeDiv[i] = i;

  for (uint64_t i = 2; i * i <= N; ++i)
    if (mLowestPrimeDiv[i] == i)
      for (uint64_t j = i * i; j <= N; j += i)
        if (mLowestPrimeDiv[j] == j) mLowestPrimeDiv[j] = i;

  for (uint64_t i = 2; i <= N; i++)
    if (mLowestPrimeDiv[i] == i) mAllPrimes.push_back(i);
}

template <uint64_t N> bool PrimeSieve<N>::is_prime(uint64_t n) const
{
  if (n == 0) throw std::invalid_argument("is prime: can't factor zero");
  if (n == 1) return false;
  if (n <= N) return mLowestPrimeDiv[n] == n;

  for (prime_t p : mAllPrimes)
    if (n % p == 0) return false;

  prime_t next = mAllPrimes.back() + 1;
  while (next * next <= n)
  {
    if (n % next == 0) return false;
    next++;
  }

  return true;
}

template <uint64_t N> void PrimeSieve<N>::emit_factors(uint64_t n, auto callback) const
{
  auto fastFactor = [&](uint64_t& n)
  {
    while (n != 1)
    {
      callback(mLowestPrimeDiv[n]);
      n /= mLowestPrimeDiv[n];
    }
  };

  if (n <= N) return fastFactor(n);

  auto checkDivisor = [&](uint64_t& n, uint64_t d)
  {
    if (n == 1) return;
    while (n != 1 && n % d == 0)
    {
      callback(d);
      n /= d;
      if (n <= N) fastFactor(n);
    }
  };

  for (prime_t p : mAllPrimes) checkDivisor(n, p);

  uint64_t next = mAllPrimes.back() + 2;
  while (next * next <= n)
  {
    checkDivisor(n, next);
    next += 2;
  }

  if (n != 1) callback(n);
}
