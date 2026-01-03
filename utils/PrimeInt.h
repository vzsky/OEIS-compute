#pragma once

#include "Prime.h"
#include <iostream>
#include <stdexcept>

class PrimeInt
{
  // support auto factorization of primes upto SmallN
  static constexpr int SmallN = 500;
  static const Prime<SmallN> factorizer;

public:
  PrimeInt() = default;
  PrimeInt(std::vector<std::pair<int, int>>&& f) : mFactors(std::move(f))
  {
  }
  PrimeInt(const std::vector<std::pair<int, int>>& f) : mFactors(f)
  {
  }
  PrimeInt(size_t v)
  {
    if (v == 0)
      throw std::invalid_argument("PrimeInt doesn't support zero");
    if (v > 1)
      mFactors = factorizer.vector_factors_freq(v);
  }

  friend std::ostream& operator<<(std::ostream& out, const PrimeInt& m);

  const PrimeInt& operator*=(const PrimeInt& other);
  PrimeInt operator*(const PrimeInt& other) const;

  const PrimeInt& operator/=(const PrimeInt& other);
  PrimeInt operator/(const PrimeInt& other) const;

  bool is_divisible_by(const PrimeInt& other) const;
  bool operator==(const PrimeInt& other) const;

private:
public:
  std::vector<std::pair<int, int>> mFactors; // pair of prime and exponent
};
