#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <utils/Prime.h>

class PrimeInt
{
  // support auto factorization of primes upto SmallN
  static constexpr size_t SmallN = 500;
  static const Prime<SmallN> factorizer;

public:
  PrimeInt() = default;
  PrimeInt(std::vector<std::pair<size_t, size_t>>&& f) : mFactors(std::move(f)) {}
  PrimeInt(const std::vector<std::pair<size_t, size_t>>& f) : mFactors(f) {}
  PrimeInt(size_t v)
  {
    if (v == 0) throw std::invalid_argument("PrimeInt doesn't support zero");
    if (v > 1) mFactors = factorizer.vector_factors_freq(v);
  }

  friend std::ostream& operator<<(std::ostream& out, const PrimeInt& m);

  const PrimeInt& operator*=(const PrimeInt& other);
  PrimeInt operator*(const PrimeInt& other) const;

  const PrimeInt& operator/=(const PrimeInt& other);
  PrimeInt operator/(const PrimeInt& other) const;

  bool is_divisible_by(const PrimeInt& other) const;
  bool operator==(const PrimeInt& other) const;

  uint64_t unsafe_to_int() const
  {
    uint64_t ans = 1;
    for (const auto& [p, m] : mFactors) ans *= std::pow(p, m);
    return ans;
  }

  const std::vector<std::pair<size_t, size_t>>& factors() const { return mFactors; }

  size_t mult_of_prime(size_t prime) const
  {
    auto it = std::lower_bound(mFactors.begin(), mFactors.end(), prime,
                               [](const auto& a, size_t v) { return a.first < v; });

    if (it == mFactors.end() || it->first != prime) return 0;
    return it->second;
  }

private:
  // sorted pair of prime and exponent
  std::vector<std::pair<size_t, size_t>> mFactors;
  std::vector<std::pair<size_t, size_t>> mTmp;
};
