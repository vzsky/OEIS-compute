#include <gmpxx.h>
#include <iostream>
#include <utils/BigInt.h>
#include <utils/Fraction.h>
#include <utils/MoreMath.h>

namespace A331373
{

template <uint32_t DIGITS> std::vector<uint16_t> get_answer_gmp()
{
  mpz_class limit;
  mpz_ui_pow_ui(limit.get_mpz_t(), 10, DIGITS + 2);

  mpz_class fact   = 1;
  mpz_class answer = 0;

  for (uint32_t k = 2; fact <= limit; ++k)
  {
    fact *= k;
    answer += limit / (fact - 1);
  }

  std::vector<uint16_t> digits;
  {
    std::string s = answer.get_str(10);
    digits.reserve(s.length());

    for (char c : s)
    {
      digits.push_back(c - '0');
    }
  }
  return digits;
}

template <uint32_t DIGITS> DecBigInt get_answer()
{
  static DenseBigInt limit = math::pow(DenseBigInt(10), DIGITS + 2);

  DenseBigInt fact(1);
  DenseBigInt answer(0);

  for (uint64_t k = 2; fact <= limit; ++k)
  {
    fact *= k;
    answer += limit / (fact - 1);
  }

  return answer;
}

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
