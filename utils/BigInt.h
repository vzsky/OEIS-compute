#pragma once

#include <ostream>
#include <string>

template <class Backend> class BigInt
{
private:
  Backend mBack;

public:
  BigInt() = default;
  BigInt(int v) : mBack(v)
  {
  }
  BigInt(const std::string& s) : mBack(s)
  {
  }

  BigInt abs() const
  {
    BigInt r = *this;
    r.mBack.mIsNeg = false;
    return r;
  }

  BigInt operator-() const
  {
    BigInt r = *this;
    if (!(r.mBack.mDigits.size() == 1 && r.mBack.mDigits[0] == 0))
      r.mBack.mIsNeg = !r.mBack.mIsNeg;
    return r;
  }

  const BigInt& operator<<=(int k)
  {
    mBack.shl(k);
    return *this;
  }

  const BigInt& operator>>=(int k)
  {
    mBack.shr(k);
    return *this;
  }

  const BigInt& operator+=(const BigInt& rhs)
  {
    mBack.add(rhs.mBack);
    return *this;
  }

  const BigInt& operator-=(const BigInt& rhs)
  {
    mBack.sub(rhs.mBack);
    return *this;
  }

  const BigInt& operator*=(const BigInt& rhs)
  {
    mBack.mul(rhs.mBack);
    return *this;
  }

  const BigInt& operator%=(const BigInt& rhs)
  {
    mBack.mod(rhs.mBack);
    return *this;
  }

  BigInt operator+(const BigInt& rhs) const
  {
    BigInt r = *this;
    r += rhs;
    return r;
  }

  BigInt operator-(const BigInt& rhs) const
  {
    BigInt r = *this;
    r -= rhs;
    return r;
  }

  BigInt operator*(const BigInt& rhs) const
  {
    BigInt r = *this;
    r *= rhs;
    return r;
  }

  BigInt operator%(const BigInt& rhs) const
  {
    BigInt r = *this;
    r %= rhs;
    return r;
  }

  std::strong_ordering operator<=>(const BigInt& o) const
  {
    if (mBack.mIsNeg != o.mBack.mIsNeg)
      return mBack.mIsNeg ? std::strong_ordering::less : std::strong_ordering::greater;

    int c = Backend::abs_cmp(mBack, o.mBack);
    if (c == 0)
      return std::strong_ordering::equal;

    bool less_abs = (c < 0);
    if (!mBack.mIsNeg)
      return less_abs ? std::strong_ordering::less : std::strong_ordering::greater;
    else
      return less_abs ? std::strong_ordering::greater : std::strong_ordering::less;
  }

  bool operator==(const BigInt& o) const
  {
    return (*this <=> o) == 0;
  }

  bool isNegative() const
  {
    return mBack.mIsNeg;
  }

  const auto& digits() const
  {
    return mBack.mDigits;
  }

  friend std::ostream& operator<<(std::ostream& os, const BigInt& b)
  {
    return os << b.mBack;
  }
};
