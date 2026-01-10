#include "lib.h"
#include <A389544/lib.h>
#include <cassert>
#include <cstdint>
#include <utils/LogInt.h>
#include <vector>

constexpr uint32_t CacheLim = 2e9;

template <uint32_t N> struct A390848 : public A389544<N, CacheLim>
{
  void _skip(uint64_t n) override
  {
    // static int ind = 1;
    // std::cout << ind++ << ' ' << n << std::endl;
    if (this->is_interesting(n)) std::cout << "INTERESTING SKIP " << n << std::endl;
  }
};

template <uint32_t N> void compute_from_scratch()
{
  A390848<N> e{};
  auto result = e.get_sequence_until_N();

  std::vector<int> skipped;
  int index = 0;
  for (int i = 2; i <= N; i++)
  {
    if (result[index] == i)
      index++;
    else
      skipped.push_back(i);
  }

  std::vector<int> reference = {
      6,    12,   16,   20,   24,   35,   56,   60,   72,   90,   110,  120,  140,  143,  182,  210,  255,
      280,  306,  342,  352,  399,  420,  462,  504,  506,  575,  650,  702,  715,  720,  756,  812,  840,
      870,  930,  990,  992,  1056, 1120, 1122, 1224, 1332, 1406, 1430, 1482, 1560, 1640, 1722, 1806, 1892,
      1980, 2002, 2070, 2162, 2256, 2352, 2450, 2520, 2550, 2652, 2730, 2756, 2862, 2970,
  };

  int min_size = std::min(reference.size(), skipped.size());
  skipped.resize(min_size);
  reference.resize(min_size);
  assert(skipped == reference);
}

int main() { utils::timeit(compute_from_scratch<1'000'000>); }
