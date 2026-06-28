#pragma once

#include <cstdint>
#include <stdexcept>
#include <utils/Prime.hpp>
#include <vector>

/* PrimeVector
 * - think of this as unordered_map keyed by prime number, except it's ordered
 * - a little more expensive than array for small ValueT, but iterable
 * */
template <size_t Bound_, typename ValueT> struct PrimeVector
{
  static constexpr size_t Bound          = Bound_;
  static constexpr uint32_t kInvalidRank = UINT32_MAX;

  explicit PrimeVector(ValueT defaultVal = ValueT{})
  {
    uint32_t rank = 0;
    PrimeSieve<Bound> sieve;
    for (uint64_t p : sieve.all_primes()) mRank[p] = rank++;
    mData.assign(rank, defaultVal);
  }

  [[nodiscard]] ValueT& operator[](uint64_t prime) noexcept { return mData[mRank[prime]]; }
  [[nodiscard]] const ValueT& operator[](uint64_t prime) const noexcept { return mData[mRank[prime]]; }

  [[nodiscard]] ValueT& at(uint64_t prime)
  {
    if (prime >= Bound || mRank[prime] == kInvalidRank)
      throw std::out_of_range("PrimeVector::at: not a prime in bound");
    return mData[mRank[prime]];
  }
  [[nodiscard]] const ValueT& at(uint64_t prime) const
  {
    if (prime >= Bound || mRank[prime] == kInvalidRank)
      throw std::out_of_range("PrimeVector::at: not a prime in bound");
    return mData[mRank[prime]];
  }

  [[nodiscard]] size_t size() const noexcept { return mData.size(); }
  [[nodiscard]] auto begin() noexcept { return mData.begin(); }
  [[nodiscard]] auto end() noexcept { return mData.end(); }
  [[nodiscard]] auto begin() const noexcept { return mData.begin(); }
  [[nodiscard]] auto end() const noexcept { return mData.end(); }

private:
  std::vector<uint32_t> mRank = std::vector<uint32_t>(Bound, kInvalidRank);
  std::vector<ValueT> mData;
};
