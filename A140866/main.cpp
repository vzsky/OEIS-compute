#include "lib.h"
#include <cmath>
#include <utils/DecBigInt.h>
#include <utils/Utils.h>

bool is_harshad(DecBigInt n)
{
  long long sum = 0;
  for (auto x : n.digits())
    sum += x;
  return (n % sum == 0);
}

long long sum_digit(int n)
{
  long long sum = 0;
  while (n != 0)
  {
    sum += (n % 10);
    n /= 10;
  }
  return sum;
}

bool is_harshad_fast(long long n)
{
  return ((n % sum_digit(n)) == 0);
}

int main()
{

  {
    int N = 1e5;
    int cnt = 0;
    int current_n = 0;
    for (int i = 1; i <= N; i++)
    {
      if (is_harshad(i))
        cnt++;
      if (i == std::pow(10, current_n))
      {
        std::cout << current_n << ' ' << cnt << std::endl;
        current_n++;
      }
    }
  }

  // {
  //   long long N = 1e10;
  //   int cnt = 0;
  //   int current_n = 0;
  //   for (long long i = 1; i <= N; i++) {
  //     if (is_harshad_fast( i )) cnt++;
  //     if (i == std::pow(10, current_n)) {
  //       std::cout << current_n << ' ' << cnt << std::endl;
  //       current_n++;
  //     }
  //   }
  // }

  // {
  //   long long N = 1e11;
  //   std::unordered_set<int> harshad;
  //   for (long long i = 1; i < N; i++) {
  //     if (is_harshad_fast(i)) harshad.insert(i);
  //     bool found = false;
  //     for (auto x : harshad) {
  //       if (harshad.contains(i - x)) { found = true; break; }
  //     }
  //     if (!found) std::cout << i << std::endl;
  //   }
  // }
}
