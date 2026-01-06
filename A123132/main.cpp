#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <utils/BigInt.h>
#include <utils/Prime.h>
#include <utils/Utils.h>

template <int N> class A123132
{
public:
  DecBigInt describe_prime_factors(int x)
  {
    std::map freq = factorizer.factors_freq(x);
    std::string out;
    for (auto &[p, cnt] : freq)
    {
      out += std::to_string(cnt);
      out += std::to_string(p);
    }
    return out;
  }

  static int digit_of(int x)
  {
    if (x == 0)
      return 1;
    return static_cast<int>(std::log10(std::abs(x))) + 1;
  }

  const Prime<N> &getFactorizer() const { return factorizer; }

private:
  Prime<N> factorizer;
};

int main()
{
  {
    constexpr int N = 1e7;
    A123132<N> a;
    auto factorizer = a.getFactorizer();

    int cnt = 0;
    for (int n = 2; n <= N; ++n)
    {
      if (n % 2 == 0)
        continue; // focusing on ==, it's impossible
      if (n % 5 == 0)
        continue; // focusing on ==, it's impossible
      if (factorizer.factors_freq(n).size() == 1)
        continue; // same

      if (n >= a.describe_prime_factors(n))
      {
        cnt++;
        std::cout << n << " -> ";

        for (auto &[p, c] : factorizer.factors_freq(n))
        {
          std::cout << p << ':' << c << ' ';
        }
        std::cout << std::endl;
      }
    }
    std::cout << "total: " << cnt << std::endl;
  }
}
