#include "libgenetic.h"
#include "libnaive.h"
#include <utils/GeneticAlg.h>
#include <utils/MetaProg.h>
#include <utils/MoreMath.h>
#include <utils/Utils.h>

#include <iostream>
#include <vector>

// min across all equal partitions, A, B
// {
//    the maximum [number of solutions of a - b = x] across all x
// }
//
// n = 1 -> {1} {2} -> x = 1, max = 1;
// n = 2 -> {1, 2}, {3, 4} -> max = 2 and {1, 3}, {2, 4} -> max = 2;
// n = 3 -> {1, 2, 3}, {4, 5, 6} -> max = 3; from [3,2,1,4,3,2,5,4,3]
//       -> {1, 2, 4}, {3, 5, 6} -> max = 2; from [2,1,-1,4,3,1,5,4,2]
//       -> {1, 2, 5}, {3, 4, 6} -> max = 2;
//
// upper bound is easier to find (only one partition needed)
//
// plan naive: ((2n C n) n^2) wouldn't go pass 30.
// - do (2n choose n) partitions, in each partition,
//   - do n^2 loops to get all sum
// - can optimize to (2n C n) n by editing sum rather than calculating new one?

int main()
{
  const auto naive_run = []
  {
    constexpr int N = 30;
    mp::For<1, N>([](auto i)
    { utils::timeit([&] { std::cout << i << ": " << naive::exact_answer<i>() << std::endl; }); });
  };

  // naive_run();

  const auto genetic_run = []
  {
    using namespace genetic;
    mp::For<1000, 1001>([&](auto N)
    {
      utils::timeit([&]
      {
        using Gene = Gene<N>;

        Gene adam = [&]
        {
          std::vector<int> mask(2 * N);
          for (int i = 0; i < (N + 1) / 2; i++) mask[i] = 1;
          for (int i = (3 * N + 1) / 2; i < 2 * N; i++) mask[i] = 1;
          assert(std::accumulate(mask.begin(), mask.end(), 0) == N);
          return Gene{std::move(mask)};
        }();

        GeneticSearcher<Gene> g{};
        g.config.elite_count     = 1000;
        g.config.population_size = 10000;
        g.setGenerationCB([](const Gene& g, int generation)
        { std::cout << g.get_score() << ' ' << g.get_max_count() << std::endl; });

        Gene best = g.search({adam}, 50).front();
        utils::print_range(best.get_mask());
        std::cout << "upperbound of a(" << N << ") is " << best.get_max_count() << std::endl;
      });
    });
  };

  genetic_run();
}
