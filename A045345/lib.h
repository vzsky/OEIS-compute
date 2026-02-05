#include <utils/BigInt.h>
#include <utils/Prime.h>
#include <vector>

namespace A045345
{

template <int N> std::vector<int> answers_upto()
{
  std::vector<int> answers;
  Prime<N> p;
  auto primes = p.all_primes();
  std::vector<slow_bigint::DecBigInt> sum(primes.size());
  for (int i = 1; i < primes.size(); i++)
  {
    sum[i] = sum[i - 1] + primes[i - 1];
  }
  for (int i = 1; i < primes.size(); i++)
  {
    if (sum[i] % i == 0)
    {
      answers.push_back(i);
    }
  }
  return answers;
}

} // namespace A045345
