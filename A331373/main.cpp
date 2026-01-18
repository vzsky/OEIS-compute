#include "BigInt.h"
#include "lib.h"
#include <utils/Utils.h>

constexpr size_t DIGITS = 250;
using namespace A331373;

int main()
{
  utils::timeit([]
  {
    Fraction<DenseBigInt> f_dense = get_fraction<DIGITS>(true);

    auto [mantissa, exp] = f_dense.expansion<DecBigInt>(DIGITS);

    std::cout << mantissa << std::endl;

    auto answer = mantissa.digits();
    std::reverse(answer.begin(), answer.end());
    auto result = utils::read_bfile<uint16_t>("./b331373.txt");
    result.resize(std::min(result.size(), DIGITS));
    answer.resize(result.size());
    assert(result == answer);
  });
}
