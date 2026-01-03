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
  void abs_add(const DecBackend&);
  void abs_sub(const DecBackend&);
  static int abs_cmp(const DecBackend&, const DecBackend&);

  void add(const DecBackend&);
  void sub(const DecBackend&);
  void mul(const DecBackend&);
  void mod(const DecBackend&);

  void shl(int);
  void shr(int);

  std::strong_ordering cmp(const DecBackend&) const;

  bool operator==(const DecBackend& o) const
  {
    return cmp(o) == std::strong_ordering::equal;
  }

private:
  bool mIsNeg{false};
  std::vector<Digit> mDigits;
};

using DecBigInt = BigInt<DecBackend>;
