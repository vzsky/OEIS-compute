#include <A062241/lib.hpp>
#include <utils/Fft.hpp>
#include <utils/MetaProg.hpp>
#include <utils/Prime.hpp>
#include <utils/Utils.hpp>

// a(n) is the smallest k such that n = a + b where a and b are not divisible by any prime > k
// equivalently, a(n) is the minimum of (maximum prime factor of a and b) across all choice of a, b

// one possible speed up is that the answer needs to be a number with the lowest prime factor higher than
// the currently searching prime
int main()
{
  int lower_bound = -1;
  const auto run  = [&]<int N, int M, int K>()
  {
    utils::ScopeTimer _t{};

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
        Log(LL::Info, k, answer);
        lower_bound = answer;
      }
    });
  };
  run.operator()<200'000, 2, 31>();
  run.operator()<2'200'000, 31, 53>();
  run.operator()<7'000'000, 53, 67>();
  run.operator()<140'000'000, 67, 83>();
}

// int main()
// {
//   const auto run_fft = []<int N, int M, int K>()
//   {
//     utils::ScopeTimer _t{};
//
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
//         Log(LL::Info, logging::detail::print_range(active));
//         auto C = fft::round<size_t>(fft::self_convolution(active));
//         Log(LL::Info, logging::detail::print_range(C));
//         auto it    = std::find(C.begin() + 2, C.begin() + N, 0);
//         int answer = (it == C.begin() + N) ? -1 : std::distance(C.begin(), it);
//         std::cout << k << ' ' << answer << std::endl;
//       }
//     });
//   };
//   run_fft.operator()<200'000, 2, 31>();
//   run_fft.operator()<2'200'000, 31, 53>();
//   run_fft.operator()<7'000'000, 53, 67>();
// }
