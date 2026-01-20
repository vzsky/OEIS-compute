#pragma once

#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

template <typename DigitT, DigitT B>
concept ValidBigIntBase = ((std::is_same_v<DigitT, uint8_t> && B >= 2 && B <= (1 << 3)) ||
                           (std::is_same_v<DigitT, uint16_t> && B >= 2 && B <= (1 << 7)) ||
                           (std::is_same_v<DigitT, uint32_t> && B >= 2 && B <= (1 << 15)) ||
                           (std::is_same_v<DigitT, uint64_t> && B >= 2 && B <= (1 << 31)));

template <typename DigitT, DigitT B>
  requires ValidBigIntBase<DigitT, B>
class BigInt
{
public:
  using Digit                 = DigitT;
  static constexpr Digit Base = B;

  static constexpr size_t KARATSUBA_THRESHOLD_DIGITS = 7000;

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

  inline BigInt operator*(const BigInt& o) const { return mult_karatsuba(o); }
  inline BigInt& operator*=(const BigInt& o)
  {
    *this = mult_karatsuba(o);
    return *this;
  }

  inline BigInt operator/(const BigInt& o) const { return divmod(o).first; }
  inline BigInt& operator/=(const BigInt& o)
  {
    *this = divmod(o).first;
    return *this;
  }

  inline BigInt operator%(const BigInt& o) const { return divmod(o).second; }
  inline BigInt& operator%=(const BigInt& o)
  {
    *this = divmod(o).second;
    return *this;
  }

  inline const std::vector<Digit>& digits() const { return mDigits; };
  inline bool is_neg() const { return mIsNeg; };
  inline bool is_zero() const { return mDigits.empty(); }

private:
  inline void normalize();
  static int abs_cmp(const BigInt& a, const BigInt& b);

  void abs_add(const BigInt& o);
  void abs_sub(const BigInt& o);
  void signed_add(const BigInt& o, bool negate_o);

  void shift_left(size_t digits);
  void shift_right(size_t digits);

  std::pair<BigInt, BigInt> divmod(const BigInt& b1) const;
  BigInt mult_simple(const BigInt& o) const;
  BigInt mult_karatsuba(const BigInt& o) const;
  void div_simple(const BigInt& o);

private:
  bool mIsNeg{false};         // 0 is not neg
  std::vector<Digit> mDigits; // 0 is {0};
};

namespace math
{
template <typename DigitT, DigitT B> BigInt<DigitT, B> pow(BigInt<DigitT, B> a, uint64_t exp);
}

using DecBigInt   = BigInt<uint16_t, 10>;
using DenseBigInt = BigInt<uint64_t, 1ull << 31>;

#include "utils/BigInt.tpp"
