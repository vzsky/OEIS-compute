#include "lib.h"
#include <cassert>

// wants a sequence such that any product u <= i <= v of s_i is unique

// greedy approach from 2
// 2, 3, 4, 5, 7, 8, 9, 10, 11 ...

int main()
{
  const int N = 300000;

  timeit(
      []()
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
            skipped.push_back(i);
        }

        std::vector<int> reference = {
            6,    12,   16,   20,   24,   35,   56,   60,   72,   90,   110,  120,  140,
            143,  182,  210,  255,  280,  306,  342,  352,  399,  420,  462,  504,  506,
            575,  650,  702,  715,  720,  756,  812,  840,  870,  930,  990,  992,  1056,
            1120, 1122, 1224, 1332, 1406, 1430, 1482, 1560, 1640, 1722, 1806, 1892, 1980,
            2002, 2070, 2162, 2256, 2352, 2450, 2520, 2550, 2652, 2730, 2756, 2862, 2970,
            3135, 3306, 3360, 3422, 3570, 3599, 3782, 3906, 4032, 4160, 4290,
        };

        int min_size = std::min(reference.size(), skipped.size());
        skipped.resize(min_size);
        reference.resize(min_size);
        assert(skipped == reference);
      });

  return 0;
}
