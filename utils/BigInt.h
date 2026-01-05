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
    return mBack.cmp(o.mBack);
  }
  bool operator==(const BigInt&) const = default;

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
