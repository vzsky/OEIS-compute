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

#define TEMPLATE_BIGINT                                                                                      \
  template <typename DigitT, DigitT B>                                                                       \
    requires ValidBigIntBase<DigitT, B>

#define BIGINT BigInt<DigitT, B>

/**************************************************************************************/

TEMPLATE_BIGINT class BigInt;
TEMPLATE_BIGINT std::ostream& operator<<(std::ostream& os, const BigInt<DigitT, B>& b);

TEMPLATE_BIGINT class BigInt
{
public:
  class View;
  using Digit                 = DigitT;
  static constexpr Digit Base = B;

  static constexpr size_t KARATSUBA_THRESHOLD_DIGITS = 64;

  BigInt();
  BigInt(int64_t v);
  BigInt(const std::string& s);
  BigInt(View v);

  template <typename ODigitT, ODigitT OB> BigInt(const BigInt<ODigitT, OB>& other);

  BigInt(const BigInt&)                = default;
  BigInt(BigInt&&) noexcept            = default;
  BigInt& operator=(const BigInt&)     = default;
  BigInt& operator=(BigInt&&) noexcept = default;

  BigInt abs() const;
  BigInt operator-() const;

  friend std::ostream& operator<< <>(std::ostream& os, const BigInt& b);
  std::strong_ordering operator<=>(const BigInt& o) const;
  inline bool operator==(const BigInt& o) const { return (*this <=> o) == std::strong_ordering::equal; };

  inline BigInt& operator+=(const BigInt& o)
  {
    signed_add_with(o, false);
    normalize();
    return *this;
  }

  inline BigInt& operator-=(const BigInt& o)
  {
    signed_add_with(o, true);
    normalize();
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

  inline BigInt operator*(const BigInt& o) const
  {
    BigInt ret = mult_karatsuba(*this, o);
    ret.normalize();
    return ret;
  }

  inline BigInt& operator*=(const BigInt& o)
  {
    *this = mult_karatsuba(*this, o);
    normalize();
    return *this;
  }

  inline BigInt operator/(const BigInt& o) const
  {
    BigInt ret = divmod(*this, o).first;
    ret.normalize();
    return ret;
  }

  inline BigInt& operator/=(const BigInt& o)
  {
    *this = divmod(*this, o).first;
    normalize();
    return *this;
  }

  inline BigInt operator%(const BigInt& o) const
  {
    BigInt ret = divmod(*this, o).second;
    ret.normalize();
    return ret;
  }

  inline BigInt& operator%=(const BigInt& o)
  {
    *this = divmod(*this, o).second;
    normalize();
    return *this;
  }

  inline const std::vector<Digit>& digits() const { return mDigits; };
  inline bool is_neg() const { return mIsNeg; };
  inline bool is_zero() const { return mDigits.empty(); }

public:
  static int abs_cmp(const BigInt& a, const BigInt& b);
  static std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b);

  void shift_left(size_t digits);
  void shift_right(size_t digits);

private:
  inline void normalize();

  void abs_add_with(const BigInt& o);
  void abs_sub_with(const BigInt& o);
  void signed_add_with(const BigInt& o, bool negate_o);

  static BigInt mult_simple(const BigInt& a, const BigInt& b);
  static BigInt mult_karatsuba(const BigInt& a, const BigInt& b);
  void div_simple(const BigInt& o);

private:
  bool mIsNeg{false};         // 0 is not neg
  std::vector<Digit> mDigits; // 0 is {0};
};

template <typename DigitT, DigitT B>
  requires ValidBigIntBase<DigitT, B>
class BigInt<DigitT, B>::View
{
  const Digit* mPtr = nullptr;
  size_t mSize      = 0;
  bool mIsNeg       = false;

public:
  View() = default;
  View(const Digit* ptr, size_t len, bool isneg) : mPtr(ptr), mSize(len), mIsNeg(isneg) {}
  View(const BigInt& x) : mPtr(x.mDigits.data()), mSize(x.mDigits.size()), mIsNeg(x.mIsNeg) {}

  const Digit& operator[](size_t i) const { return mPtr[i]; }
  const Digit* data() const { return mPtr; }
  size_t size() const { return mSize; }
  bool is_neg() const { return mIsNeg; }

  View subview(size_t offset, size_t len) const
  {
    if (offset >= mSize) return View(nullptr, 0, false);
    return View(mPtr + offset, std::min(len, mSize - offset), mIsNeg);
  }
};

using DecBigInt      = BigInt<uint16_t, 10>;
using DenseDecBigInt = BigInt<uint64_t, 1'000'000'000>;
using DenseBigInt    = BigInt<uint64_t, 1ull << 31>;

#include "utils/BigInt.tpp"
