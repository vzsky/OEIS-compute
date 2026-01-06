#include "lib.h"
#include <A389544/lib.h>
#include <cassert>
#include <cstdint>
#include <vector>

template <uint64_t N> void compute_from_scratch()
{
  A389544<N> e{};
  auto result = e.get_sequence_until_N();

  std::vector<int> skipped;
  int index = 0;
  for (int i = 2; i <= N; i++)
  {
    if (result[index] == i)
      index++;
    else
    {
      skipped.push_back(i);
      std::cout << i << '\n';
    }
  }

  std::vector<int> reference = {
      6,    12,   16,   20,   24,   35,   56,   60,   72,   90,   110,
      120,  140,  143,  182,  210,  255,  280,  306,  342,  352,  399,
      420,  462,  504,  506,  575,  650,  702,  715,  720,  756,  812,
      840,  870,  930,  990,  992,  1056, 1120, 1122, 1224, 1332, 1406,
      1430, 1482, 1560, 1640, 1722, 1806, 1892, 1980, 2002, 2070, 2162,
      2256, 2352, 2450, 2520, 2550, 2652, 2730, 2756, 2862, 2970,
  };

  int min_size = std::min(reference.size(), skipped.size());
  skipped.resize(min_size);
  reference.resize(min_size);
  assert(skipped == reference);
}

int main()
{
  // utils::timeit(compute_from_scratch<300000>); // 6 hours

  {
    constexpr int N = 300000;
    A389544<N> e{};
    std::vector<uint64_t> skipped =
        utils::read_bfile<uint64_t>("./b390848.txt");
    e.print_interesting(skipped);
  }
}
