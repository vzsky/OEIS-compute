#include <iostream>
#include <utils/BigInt.h>
#include <utils/Fraction.h>
#include <utils/MoreMath.h>

namespace A331373
{

template <typename BigIntType, uint32_t DIGITS> BigIntType get_answer()
{
  static BigIntType limit = math::pow(BigIntType(10), DIGITS + 2);

  BigIntType fact   = 1;
  BigIntType answer = 0;

  for (uint32_t k = 2; fact <= limit; ++k)
  {
    fact *= k;
    answer += limit / (fact - 1);
  }

  return answer;
}

// testing fraction
using slow_bigint::DenseBigInt;
template <uint32_t DIGITS> Fraction<DenseBigInt> get_fraction(bool stats = false)
{
  DenseBigInt limit = math::pow(DenseBigInt(10), DIGITS + 2);

  Fraction<DenseBigInt> f;
  DenseBigInt fact(1);

  for (uint64_t k = 2;; ++k)
  {
    fact *= k;
    DenseBigInt d = fact - 1;

    if (d > limit)
    {
      if (stats) std::cout << "Calculated until k=" << k << " prefix sum" << std::endl;

      fact *= k + 1;
      break;
    }

    f += Fraction<DenseBigInt>{1, d};
  }

  return f;
}

} // namespace A331373
