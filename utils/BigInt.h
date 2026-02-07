#pragma once

#include <compare>
#include <gmpxx.h>
#include <iostream>
#include <string>

class BigInt
{
public:
  BigInt() : mValue(0) {}
  BigInt(long v) : mValue(v) {}
  BigInt(const std::string& s) : mValue(s) {}

  explicit BigInt(mpz_class v) : mValue(std::move(v)) {}

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

  BigInt abs() const { return BigInt(::abs(mValue)); }

  BigInt& operator+=(const BigInt& o)
  {
    mValue += o.mValue;
    return *this;
  }
  BigInt& operator-=(const BigInt& o)
  {
    mValue -= o.mValue;
    return *this;
  }
  BigInt& operator*=(const BigInt& o)
  {
    mValue *= o.mValue;
    return *this;
  }
  BigInt& operator/=(const BigInt& o)
  {
    mValue /= o.mValue;
    return *this;
  }
  BigInt& operator%=(const BigInt& o)
  {
    mValue %= o.mValue;
    return *this;
  }

  friend BigInt operator+(BigInt a, const BigInt& b)
  {
    a += b;
    return a;
  }
  friend BigInt operator-(BigInt a, const BigInt& b)
  {
    a -= b;
    return a;
  }
  friend BigInt operator*(BigInt a, const BigInt& b)
  {
    a *= b;
    return a;
  }
  friend BigInt operator/(BigInt a, const BigInt& b)
  {
    a /= b;
    return a;
  }

  friend std::ostream& operator<<(std::ostream& os, const BigInt& b) { return os << b.mValue; }

private:
  mpz_class mValue;
};

#include <utils/SlowBigInt.tpp>
