#pragma once

#include <compare>
#include <cstdint>
#include <gmpxx.h>
#include <iostream>
#include <string>
#include <vector>

class BigInt
{
  explicit BigInt(mpz_class v) : mValue(std::move(v)) {}

public:
  static constexpr uint8_t Base = 10;

  BigInt() : mValue(0) {}

  template <typename T>
    requires std::is_signed_v<T>
  BigInt(T v)
  {
    mpz_set_si(mValue.get_mpz_t(), static_cast<int64_t>(v));
  }

  template <typename T>
    requires std::is_unsigned_v<T>
  BigInt(T v)
  {
    mpz_set_ui(mValue.get_mpz_t(), static_cast<uint64_t>(v));
  }

  BigInt(const std::string& s, int base = 10) : mValue(s, base) {}

  auto operator<=>(const BigInt& o) const
  {
    int cmp = mpz_cmp(mValue.get_mpz_t(), o.mValue.get_mpz_t());
    if (cmp < 0) return std::strong_ordering::less;
    if (cmp > 0) return std::strong_ordering::greater;
    return std::strong_ordering::equal;
  }
  bool operator==(const BigInt& o) const { return mValue == o.mValue; }

  bool is_neg() const { return mpz_sgn(mValue.get_mpz_t()) < 0; }
  bool is_zero() const { return mpz_sgn(mValue.get_mpz_t()) == 0; }

  std::vector<uint8_t> digits() const
  {
    if (is_zero()) return {0};
    std::string s = mValue.get_str(10);
    std::vector<uint8_t> d;
    d.reserve(s.size());
    for (char c : s)
      if (isdigit(c)) d.push_back(c - '0');
    return d;
  }

  BigInt abs() const { return BigInt(::abs(mValue)); }

#define MAKE_BINARY_OP(op)                                                                                   \
  BigInt& operator op## = (const BigInt& o)                                                                  \
  {                                                                                                          \
    mValue op## = o.mValue;                                                                                  \
    return *this;                                                                                            \
  }                                                                                                          \
  friend BigInt operator op(BigInt a, const BigInt& b)                                                       \
  {                                                                                                          \
    a op## = b;                                                                                              \
    return a;                                                                                                \
  }

  MAKE_BINARY_OP(+);
  MAKE_BINARY_OP(-);
  MAKE_BINARY_OP(*);
  MAKE_BINARY_OP(/);
  MAKE_BINARY_OP(%);

  MAKE_BINARY_OP(&);
  MAKE_BINARY_OP(|);
  MAKE_BINARY_OP(^);

#undef MAKE_BINARY_OP

  BigInt operator-() const
  {
    BigInt result = *this;
    mpz_neg(result.mValue.get_mpz_t(), result.mValue.get_mpz_t());
    return result;
  }

  friend std::ostream& operator<<(std::ostream& os, const BigInt& b) { return os << b.mValue; }

private:
  mpz_class mValue;
};

#include <utils/SlowBigInt.tpp>
