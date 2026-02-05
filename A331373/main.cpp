#include "lib.h"
#include <utils/BigInt.h>
#include <utils/Utils.h>

constexpr size_t DIGITS = 10050;
using namespace A331373;

int main()
{ // power of gmp!
  // utils::timeit([]
  // {
  //   auto answer = get_answer_gmp<DIGITS>();
  //   auto result = utils::read_bfile<uint16_t>("./b331373.txt");
  //   result.resize(std::min(result.size(), DIGITS));
  //   answer.resize(result.size());
  //   assert(result == answer);
  // });

  utils::timeit([]
  {
    auto r      = get_answer<DIGITS>();
    auto answer = ((DecBigInt)r).digits();
    std::reverse(answer.begin(), answer.end());
    auto result = utils::read_bfile<uint16_t>("./b331373.txt");
    result.resize(std::min(result.size(), DIGITS));
    answer.resize(result.size());
    assert(result == answer);
  });
}
