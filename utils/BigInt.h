#pragma once

#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

template <typename DigitT, DigitT B> class BigInt
{
public:
  using Digit                 = DigitT;
  static constexpr Digit Base = B;

  BigInt();
  BigInt(int64_t v);
  BigInt(const std::string& s);

  template <typename ODigitT, ODigitT OB> BigInt(const BigInt<ODigitT, OB>& other);

  BigInt(const BigInt&)                = default;
  BigInt(BigInt&&) noexcept            = default;
  BigInt& operator=(const BigInt&)     = default;
  BigInt& operator=(BigInt&&) noexcept = default;

  BigInt abs() const;
  BigInt operator-() const;

  std::strong_ordering operator<=>(const BigInt& o) const;
  inline bool operator==(const BigInt& o) const { return (*this <=> o) == std::strong_ordering::equal; };

  friend std::ostream& operator<<(std::ostream& os, const BigInt& b)
  {
    if (b.mIsNeg) os << "-";
    for (auto it = b.mDigits.rbegin(); it != b.mDigits.rend(); ++it)
      if constexpr (Base <= 10)
        os << *it;
      else
        os << "(" << *it << ")";
    return os;
  }

  inline BigInt& operator+=(const BigInt& o)
  {
    signed_add(o, false);
    return *this;
  }

  inline BigInt& operator-=(const BigInt& o)
  {
    signed_add(o, true);
    return *this;
  }

  inline BigInt operator+(const BigInt& o) const
  {
    BigInt r = *this;
    r += o;
    return r;
  }

  inline BigInt operator-(const BigInt& o) const
  {
    BigInt r = *this;
    r -= o;
    return r;
  }

  inline BigInt operator*(const BigInt& o) const { return mult_simple(o); }
  inline BigInt& operator*=(const BigInt& o)
  {
    *this = mult_simple(o);
    return *this;
  }

  inline BigInt operator/(const BigInt& o) const { return divmod(*this, o).first; }
  inline BigInt& operator/=(const BigInt& o)
  {
    *this = divmod(*this, o).first;
    return *this;
  }

  inline BigInt operator%(const BigInt& o) const { return divmod(*this, o).second; }
  inline BigInt& operator%=(const BigInt& o)
  {
    *this = divmod(*this, o).second;
    return *this;
  }

  inline const std::vector<Digit>& digits() const { return mDigits; };
  inline bool is_neg() const { return mIsNeg; };
  inline bool is_zero() const;

private:
  inline void normalize();
  static int abs_cmp(const BigInt& a, const BigInt& b);

  void abs_add(const BigInt& o);
  void abs_sub(const BigInt& o);
  void signed_add(const BigInt& o, bool negate_o);

  static std::pair<BigInt, BigInt> divmod(const BigInt& a1, const BigInt& b1);
  BigInt mult_simple(const BigInt& o) const;
  BigInt abs_mod_div(const BigInt& o);

private:
  bool mIsNeg{false};
  std::vector<Digit> mDigits;

  void mul_small(uint64_t k)
  {
    if (k == 0)
    {
      mDigits.assign(1, 0);
      mIsNeg = false;
      return;
    }

    uint64_t carry = 0;
    for (size_t i = 0; i < mDigits.size(); ++i)
    {
      uint64_t cur = uint64_t(mDigits[i]) * k + carry;
      mDigits[i]   = cur % Base;
      carry        = cur / Base;
    }

    if (carry) mDigits.push_back(carry);
  }
};

namespace math
{
template <typename DigitT, DigitT B> BigInt<DigitT, B> pow(BigInt<DigitT, B> a, uint64_t exp);
}

using DecBigInt   = BigInt<uint16_t, 10>;
using DenseBigInt = BigInt<uint64_t, 1ull << 30>; // don't go higher!

#include "utils/BigInt.tpp"
