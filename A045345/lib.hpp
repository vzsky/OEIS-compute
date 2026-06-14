#pragma once

#include <utils/BigInt.hpp>
#include <utils/Prime.hpp>
#include <vector>

namespace A045345 {

template <int N> std::vector<int> answers_upto()
{
  std::vector<int> answers;
  PrimeSieve<N> p;
  auto primes = p.all_primes();
  std::vector<BigInt> sum(primes.size());
  for (size_t i = 1; i < primes.size(); i++)
  {
    sum[i] = sum[i - 1] + primes[i - 1];
  }

  for (size_t i = 1; i < sum.size(); i++)
  {
    if (sum[i] % i == 0)
    {
      answers.push_back(i);
    }
  }
  return answers;
}

} // namespace A045345
