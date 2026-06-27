#pragma once

#include <cstdint>
#include <vector>

template <size_t Bound_, typename ValueT> struct PrimeVector
{
  static constexpr size_t Bound = Bound_;

  explicit PrimeVector(ValueT defaultVal = ValueT{})
  {
    std::vector<bool> sieve(Bound, true);
    sieve[0] = sieve[1] = false;
    for (uint64_t i = 2; i * i < Bound; i++)
      if (sieve[i])
        for (uint64_t j = i * i; j < Bound; j += i) sieve[j] = false;
    uint32_t rank = 0;
    for (uint64_t i = 2; i < Bound; i++)
      if (sieve[i]) mRank[i] = rank++;
    mData.assign(rank, defaultVal);
  }

  [[nodiscard]] ValueT& operator[](uint64_t prime) noexcept { return mData[mRank[prime]]; }
  [[nodiscard]] const ValueT& operator[](uint64_t prime) const noexcept { return mData[mRank[prime]]; }

private:
  std::vector<uint32_t> mRank = std::vector<uint32_t>(Bound, 0);
  std::vector<ValueT> mData;
};

template <uint64_t PrimeBound> struct FastPrimeSieve
{
  FastPrimeSieve()
  {
    std::vector<bool> sieve(PrimeBound + 1, true);
    sieve[0] = sieve[1] = false;
    for (uint64_t i = 2; i * i <= PrimeBound; ++i)
      if (sieve[i])
        for (uint64_t j = i * i; j <= PrimeBound; j += i) sieve[j] = false;
    for (uint64_t i = 2; i <= PrimeBound; ++i)
      if (sieve[i]) mPrimes.push_back(i);
  }

  [[nodiscard]] std::vector<uint64_t> distinct_factors(uint64_t n) const
  {
    std::vector<uint64_t> result;
    for (uint64_t p : mPrimes)
    {
      if (p * p > n) break;
      if (n % p == 0)
      {
        result.push_back(p);
        while (n % p == 0) n /= p;
      }
    }

    if (n > 1 && n <= PrimeBound)
    {
      result.push_back(n);
      return result;
    }

    for (uint64_t d = PrimeBound + 1 + (PrimeBound % 2); d * d <= n; d += 2)
    {
      if (n % d == 0)
      {
        result.push_back(d);
        while (n % d == 0) n /= d;
      }
    }

    if (n > 1) result.push_back(n);

    return result;
  }

private:
  std::vector<uint64_t> mPrimes;
};
