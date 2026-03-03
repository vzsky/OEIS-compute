#include "lib.h"
#include <utils/Fft.h>
#include <utils/MetaProg.h>
#include <utils/Prime.h>
#include <utils/Utils.h>

// a(n) is the smallest k such that n = a + b where a and b are not divisible by any prime > k
// equivalently, a(n) is the minimum of (maximum prime factor of a and b) across all choice of a, b

// one possible speed up is that the answer needs to be a number with the lowest prime factor higher than
// the currently searching prime
int main()
{
  int lower_bound = -1;
  const auto run  = [&]<int N, int M, int K>()
  {
    DataHelper<N> data{};
    std::vector<char> active(N, 0);
    mp::For<2, M>([&](auto k)
    {
      if (data.prime().is_prime(k))
        for (int x : data.groupByHighestDiv().at(k)) active[x] = 1;
    });
    mp::For<M, K>([&](auto k)
    {
      if (data.prime().is_prime(k))
      {
        for (int x : data.groupByHighestDiv().at(k)) active[x] = 1;
        int answer = find_answer(data, k, lower_bound, active);
        std::cout << k << ' ' << answer << std::endl;
        lower_bound = answer;
      }
    });
  };
  utils::timeit([&] { run.operator()<200'000, 2, 31>(); });
  utils::timeit([&] { run.operator()<2'200'000, 31, 53>(); });
  utils::timeit([&] { run.operator()<7'000'000, 53, 67>(); });
  utils::timeit([&] { run.operator()<140'000'000, 67, 83>(); });
}

// int main()
// {
//   const auto run_fft = []<int N, int M, int K>()
//   {
//     DataHelper<N> data{};
//     std::vector<size_t> active(N, 0);
//
//     mp::For<2, M>([&](auto k)
//     {
//       if (data.prime().is_prime(k))
//         for (int x : data.groupByHighestDiv().at(k)) active[x] = 1;
//     });
//
//     mp::For<M, K>([&](auto k)
//     {
//       if (data.prime().is_prime(k))
//       {
//         for (int x : data.groupByHighestDiv().at(k)) active[x] = 1;
//         // utils::print_range(active);
//         auto C = fft::round<size_t>(fft::self_convolution(active));
//         // utils::print_range(C);
//         auto it    = std::find(C.begin() + 2, C.begin() + N, 0);
//         int answer = (it == C.begin() + N) ? -1 : std::distance(C.begin(), it);
//         std::cout << k << ' ' << answer << std::endl;
//       }
//     });
//   };
//   utils::timeit([&] { run_fft.operator()<200'000, 2, 31>(); });
//   utils::timeit([&] { run_fft.operator()<2'200'000, 31, 53>(); });
//   utils::timeit([&] { run_fft.operator()<7'000'000, 53, 67>(); });
// }
