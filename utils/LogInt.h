#pragma once

#include <cstdint>
#include <stdexcept>
#include <utils/PrimeInt.h>

class LogInt
{
public:
  LogInt() { mLower = mUpper = 0; }
  LogInt(uint64_t n)
  {
    if (n == 0) throw std::invalid_argument("LogInt not support 0");
    double l = std::log2(n);
    mLower   = static_cast<uint64_t>(std::floor(l));
    mUpper   = static_cast<uint64_t>(std::ceil(l));
  }

  LogInt(PrimeInt pi)
  {
    mLower = mUpper = 0;
    for (auto [p, m] : pi.factors())
    {
      mLower += static_cast<uint64_t>(m * std::floor(std::log2(p)));
      mUpper += static_cast<uint64_t>(m * std::ceil(std::log2(p)));
    }
  }

  LogInt& operator*=(const LogInt& o)
  {
    mLower += o.mLower;
    mUpper += o.mUpper;
    return *this;
  }

  LogInt operator*(const LogInt& o) const
  {
    LogInt r = *this;
    r *= o;
    return r;
  }

  bool surely_lt(const LogInt& o) const { return mUpper < o.mLower; }

  friend std::ostream& operator<<(std::ostream& os, const LogInt& o)
  {
    return os << "2^[" << o.mLower << " " << o.mUpper << "]";
  }

protected:
  long double mLower = 0;
  long double mUpper = 0;
};
