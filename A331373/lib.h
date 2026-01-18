#include <iostream>
#include <utils/BigInt.h>
#include <utils/Fraction.h>

namespace A331373
{

template <uint32_t DIGITS> Fraction<DenseBigInt> get_fraction(bool stats = false)
{
  DenseBigInt limit = 10;
  limit             = math::pow(limit, DIGITS + 1);

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
