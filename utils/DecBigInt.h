#pragma once

#include <cstdint>
#include <string>
#include <utils/BigInt.h>
#include <vector>

class DecBackend
{
public:
  using Digit = uint8_t;
  friend BigInt<DecBackend>;

private:
  DecBackend();
  DecBackend(int);
  DecBackend(const std::string&);

  void normalize();
  void abs_add(const DecBackend&); // ignore sign
  void abs_sub(const DecBackend&); // ignore sign
  static int abs_cmp(const DecBackend&, const DecBackend&);

  void add(const DecBackend&);
  void sub(const DecBackend&);
  void mul(const DecBackend&);
  void mod(const DecBackend&);

  void shl(int);
  void shr(int);

  friend inline std::ostream& operator<<(std::ostream& os, const DecBackend& b)
  {
    if (b.mIsNeg)
      os << "-";
    for (auto it = b.mDigits.rbegin(); it != b.mDigits.rend(); ++it)
      os << static_cast<int>(*it);
    return os;
  }

private:
  bool mIsNeg{false};
  std::vector<Digit> mDigits;
};

using DecBigInt = BigInt<DecBackend>;
