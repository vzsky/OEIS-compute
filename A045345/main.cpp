#include "lib.h"
#include <utils/Utils.h>

int main()
{
  timeit(
      []()
      {
        constexpr int N = 1e5;
        Prime<N> p;
        auto primes = p.all_primes();
        std::vector<BigInt> sum(primes.size());
        for (int i = 1; i < primes.size(); i++)
        {
          sum[i] = sum[i - 1] + primes[i - 1];
        }

        for (int i = 1; i < primes.size(); i++)
        {
          if (sum[i] % i == 0)
          {
            std::cout << i << ' ' << sum[i] << std::endl;
          }
        }
      });
}
