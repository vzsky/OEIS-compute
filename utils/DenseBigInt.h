#pragma once

#include <cstdint>
#include <utils/BigInt.h>
#include <vector>

// base = 256 (uint8_t)

class DenseBackend
{
public:
  using Digit = uint8_t;
  friend BigInt<DenseBackend>;

private:
  DenseBackend();
  DenseBackend(int);

  void normalize();
  void abs_add(const DenseBackend&);
  void abs_sub(const DenseBackend&);
  static int abs_cmp(const DenseBackend&, const DenseBackend&);

  void add(const DenseBackend&);
  void sub(const DenseBackend&);
  void mul(const DenseBackend&);
  void mod(const DenseBackend&);

  void shl(int);
  void shr(int);

  friend inline std::ostream& operator<<(std::ostream& os, const DenseBackend& b)
  {
    if (b.mIsNeg)
      os << "-";
    os << "{";
    for (auto it = b.mDigits.rbegin(); it != b.mDigits.rend(); ++it)
      os << static_cast<int>(*it) << ", ";
    return os << "}";
  }

private:
  bool mIsNeg{false};
  std::vector<Digit> mDigits;
};

using DenseBigInt = BigInt<DenseBackend>;
