#include "lib.h"
#include <utils/BigInt.h>
#include <utils/Fraction.h>
#include <utils/Utils.h>

constexpr size_t DIGITS = 100;

Fraction<DenseBigInt> get_answer()
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
      std::cout << "Calculated " << k << " terms" << std::endl;
      break;
    }

    f += Fraction<DenseBigInt>{1, d};
  }

  return f;
}

int main()
{
  utils::timeit([]
  {
    Fraction<DenseBigInt> f_dense = get_answer();

    auto [mantissa, exp] = f_dense.expansion<DecBigInt>(DIGITS);

    std::cout << mantissa << std::endl;

    // auto answer = mantissa.digits();
    // std::reverse(answer.begin(), answer.end());
    // auto result = utils::read_bfile<uint16_t>("./b.txt");
    // result.resize(std::min(result.size(), DIGITS));
    // answer.resize(result.size());
    // assert(result == answer);
  });
}
