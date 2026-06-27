#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <utils/Logging.hpp>
#include <utils/Prime.hpp>

class PrimeInt
{
  // support auto factorization of primes using Prime<SmallN>
  static constexpr size_t SmallN = 500;
  static const PrimeSieve<SmallN> factorizer;

public:
  PrimeInt() = default;
  PrimeInt(std::vector<std::pair<uint64_t, uint64_t>>&& f) : mFactors(std::move(f)) {}
  PrimeInt(const std::vector<std::pair<uint64_t, uint64_t>>& f) : mFactors(f) {}
  PrimeInt(size_t v)
  {
    Log(LL::Infra, "Constructing prime int with Prime<N=$> factorization"_f, SmallN);
    if (v == 0) throw std::invalid_argument("PrimeInt doesn't support zero");
    if (v > 1) mFactors = factorizer.vector_factors_freq(v);
  }

  friend std::ostream& operator<<(std::ostream& out, const PrimeInt& m);

  const PrimeInt& operator*=(const PrimeInt& other);
  [[nodiscard]] PrimeInt operator*(const PrimeInt& other) const;

  const PrimeInt& operator/=(const PrimeInt& other);
  [[nodiscard]] PrimeInt operator/(const PrimeInt& other) const;

  [[nodiscard]] bool is_divisible_by(const PrimeInt& other) const;
  [[nodiscard]] bool operator==(const PrimeInt& other) const;

  [[nodiscard]] uint64_t unsafe_to_int() const
  {
    Log(LL::Infra, "Converting primeInt to uint64_r, may overflow"_f);
    uint64_t ans = 1;
    for (const auto& [p, m] : mFactors) ans *= std::pow(p, m);
    return ans;
  }

  [[nodiscard]] const std::vector<std::pair<uint64_t, uint64_t>>& factors() const { return mFactors; }

private:
  // sorted pair of prime and exponent
  std::vector<std::pair<uint64_t, uint64_t>> mFactors;
  std::vector<std::pair<uint64_t, uint64_t>> mTmp;
};
