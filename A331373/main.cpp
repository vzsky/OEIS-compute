#include "lib.h"
#include <ranges>
#include <utils/BigInt.h>
#include <utils/Utils.h>

constexpr size_t DIGITS = 5000;
using namespace A331373;

int main()
{ // still slower than a simple python :(
  utils::timeit([]
  {
    Fraction<DenseBigInt> f_dense = get_fraction<DIGITS>(true);

    std::cout << "numerator is " << f_dense.numerator().digits().size() << " digits in base "
              << DenseBigInt::Base << std::endl;

    auto mantissa = f_dense.expansion<DecBigInt>(DIGITS);

    auto answer = mantissa.digits();
    std::reverse(answer.begin(), answer.end());
    auto result = utils::read_bfile<uint16_t>("./b331373.txt");
    result.resize(std::min(result.size(), DIGITS));
    answer.resize(result.size());
    assert(result == answer);

    for (auto d : mantissa.digits() | std::views::reverse)
    {
      static int i = 0;
      std::cout << ++i << ' ' << d << std::endl;
    }
  });
}
