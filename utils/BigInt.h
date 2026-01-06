#pragma once

#include <algorithm>
#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

template <uint16_t B> class BigInt
{
public:
  using Digit                    = uint16_t;
  static constexpr uint16_t Base = B;

private:
  void normalize()
  {
    while (mDigits.size() > 1 && mDigits.back() == 0) mDigits.pop_back();
    if (mDigits.size() == 1 && mDigits[0] == 0) mIsNeg = false;
  }

  // compare the underlying without sign change
  static int abs_cmp(const BigInt &a, const BigInt &b)
  {
    if (a.mDigits.size() != b.mDigits.size())
      return a.mDigits.size() < b.mDigits.size() ? -1 : 1;

    for (int i = (int)a.mDigits.size() - 1; i >= 0; --i)
    {
      if (a.mDigits[i] != b.mDigits[i])
        return a.mDigits[i] < b.mDigits[i] ? -1 : 1;
    }
    return 0;
  }

  // modify the underlying without sign change
  void abs_add(const BigInt &o)
  {
    size_t n = std::max(mDigits.size(), o.mDigits.size());
    mDigits.resize(n, 0);

    uint64_t carry = 0;
    for (size_t i = 0; i < n || carry; ++i)
    {
      if (i == mDigits.size()) mDigits.push_back(0);

      uint64_t sum =
          carry + mDigits[i] + (i < o.mDigits.size() ? o.mDigits[i] : 0);
      mDigits[i] = sum % Base;
      carry      = sum / Base;
    }
  }

  // modify the underlying without sign change
  void abs_sub(const BigInt &o)
  {
    int64_t carry = 0;
    for (size_t i = 0; i < o.mDigits.size() || carry; ++i)
    {
      int64_t cur = int64_t(mDigits[i]) -
                    (i < o.mDigits.size() ? o.mDigits[i] : 0) - carry;
      carry = cur < 0;
      if (carry) cur += Base;
      mDigits[i] = cur;
    }
    normalize();
  }

public:
  BigInt() { mDigits = {0}; }

  BigInt(int v)
  {
    mIsNeg = v < 0;
    if (v < 0) v = -v;

    if (v == 0)
    {
      mDigits = {0};
      return;
    }

    while (v > 0)
    {
      mDigits.push_back(v % Base);
      v /= Base;
    }
  }

  BigInt(const std::string &s)
  {
    mDigits = {0};
    for (char c : s)
    {
      if (c < '0' || c > '9') continue;
      *this *= 10;
      *this += (c - '0');
    }
    mIsNeg = !s.empty() && s[0] == '-';
  }

  BigInt abs() const
  {
    BigInt r = *this;
    r.mIsNeg = false;
    return r;
  }

  BigInt operator-() const
  {
    if (*this == 0) return *this;
    BigInt r = *this;
    r.mIsNeg = !r.mIsNeg;
    return r;
  }

  BigInt &operator+=(const BigInt &o)
  {
    if (mIsNeg == o.mIsNeg)
      abs_add(o);
    else
    {
      int c = abs_cmp(*this, o);
      if (c == 0)
        return 0;
      else if (c > 0)
        abs_sub(o);
      else
      {
        BigInt tmp = o;
        tmp.abs_sub(*this);
        *this = tmp;
      }
    }
    return *this;
  }

  BigInt &operator-=(const BigInt &o)
  {
    *this += -o;
    return *this;
  }

  BigInt &operator*=(const BigInt &o)
  {
    std::vector<uint64_t> tmp(mDigits.size() + o.mDigits.size(), 0);

    for (size_t i = 0; i < mDigits.size(); ++i)
      for (size_t j = 0; j < o.mDigits.size(); ++j)
        tmp[i + j] += uint64_t(mDigits[i]) * o.mDigits[j];

    mDigits.resize(tmp.size());

    uint64_t carry = 0;
    for (size_t i = 0; i < tmp.size(); ++i)
    {
      tmp[i] += carry;
      mDigits[i] = tmp[i] % Base;
      carry      = tmp[i] / Base;
    }

    mIsNeg ^= o.mIsNeg;
    normalize();
    return *this;
  }

  BigInt &operator%=(const BigInt &o)
  {
    while (mIsNeg) *this += o;
    while (abs_cmp(*this, o) >= 0) abs_sub(o);
    return *this;
  }

  BigInt operator+(const BigInt &o)
  {
    BigInt r = *this;
    return r += o;
  }
  BigInt operator-(const BigInt &o)
  {
    BigInt r = *this;
    return r -= o;
  }
  BigInt operator*(const BigInt &o)
  {
    BigInt r = *this;
    return r *= o;
  }
  BigInt operator%(const BigInt &o)
  {
    BigInt r = *this;
    return r %= o;
  }

  std::strong_ordering operator<=>(const BigInt &o) const
  {
    if (mIsNeg != o.mIsNeg)
      return mIsNeg ? std::strong_ordering::less
                    : std::strong_ordering::greater;

    int c = abs_cmp(*this, o);
    if (c == 0) return std::strong_ordering::equal;

    bool less = c < 0;
    if (!mIsNeg)
      return less ? std::strong_ordering::less : std::strong_ordering::greater;
    else
      return less ? std::strong_ordering::greater : std::strong_ordering::less;
  }

  bool operator==(const BigInt &o) const
  {
    return (*this <=> o) == std::strong_ordering::equal;
  };

  friend std::ostream &operator<<(std::ostream &os, const BigInt &b);

  const std::vector<Digit> digits() const { return mDigits; }

private:
  bool mIsNeg{false};
  std::vector<Digit> mDigits;
};

template <uint16_t Base>
std::ostream &operator<<(std::ostream &os, const BigInt<Base> &b)
{
  if (b.mIsNeg) os << "-";
  for (auto it = b.mDigits.rbegin(); it != b.mDigits.rend(); ++it)
    if constexpr (Base <= 10)
      os << *it;
    else
      os << "(" << *it << ")";
  return os;
}

using DecBigInt   = BigInt<10>;
using DenseBigInt = BigInt<256>;
