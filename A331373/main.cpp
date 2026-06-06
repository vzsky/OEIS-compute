#include <A331373/lib.hpp>
#include <utils/BigInt.hpp>
#include <utils/Utils.hpp>

constexpr std::string REFERENCE_BFILE = "./b331373.txt";
constexpr size_t DIGITS               = 3500;
using namespace A331373;

using slow_bigint::DecBigInt;
using slow_bigint::DenseBigInt;

template <typename T> void assert_compatible(std::vector<T> answer)
{
  auto b = utils::read_bfile<T>(REFERENCE_BFILE);
  b.resize(std::min(b.size(), DIGITS));
  answer.resize(b.size());
  assert(answer == b);
}

int main()
{
  // power of gmp!
  {
    utils::ScopeTimer _t{};
    auto answer = get_answer<BigInt, DIGITS>().digits();
    assert_compatible(answer);
  }

  {
    utils::ScopeTimer _t{};
    auto r      = get_answer<DenseBigInt, DIGITS>();
    auto answer = ((DecBigInt)r).digits();
    std::reverse(answer.begin(), answer.end());
    assert_compatible(answer);
  }
}
