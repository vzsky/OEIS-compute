#include "Utils.h"
#include "lib.h"
#include <A389544/lib.h>
#include <cassert>
#include <cstdint>
#include <utils/LogInt.h>

constexpr uint32_t CacheLim = 2e9;

template <uint32_t N> struct A390848 : public A389544<N, CacheLim>
{
  void _skip(uint64_t n) override
  {
    static int ind = 1;
    std::cout << ind++ << ' ' << n << std::endl;
    if (this->is_interesting(n)) std::cout << "INTERESTING SKIP " << n << std::endl;
  }
};

int main()
{
  // utils::timeit([]()
  // {
  //   A390848<101'000'000> e{};
  //   e.get_sequence_until_N();
  // });

  utils::timeit([]()
  {
    A390848<101'000'000> e{};
    // e.read_skipped(utils::read_bfile<uint64_t>("./b390848.txt"));
    e.get_sequence_until_N();
  });
}
