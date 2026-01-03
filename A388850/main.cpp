#include "lib.h"
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <utils/Utils.h>

Prime<100000000> p;

// prime: 3^2 = 9           -> 1 8
// prime: 7^2 = 49          -> 2 48
// prime: 41^2 = 1681       -> 3 1680
// prime: 277^2 = 76729     -> 4 76725
// prime: 577^2 = 332929    -> 5 332925
// prime: 2131^2 = 4541161  -> 6 4541155
// prime: 2131^2 = 4541161  -> 7 4541154
// prime: 8663^2 = 75047569 -> 8 75047565
// it seems that all answers so far are (p^2 - k) where p is the largest prime divisor

struct MaxPrime
{
  size_t maxP, mult;

  MaxPrime(std::vector<std::pair<int, int>> factors)
  {
    maxP = factors.back().first;
    mult = factors.back().second;
  }

  const MaxPrime& operator*=(const MaxPrime& other)
  {
    if (other.maxP == maxP)
      mult += other.mult;
    if (other.maxP > maxP)
    {
      maxP = other.maxP;
      mult = other.mult;
    }
    return *this;
  }
};

int main()
{
  // timeit([](){
  //   uint64_t u = 1;
  //   for (int distance = 1; distance < 10; distance ++) {
  //     while (u++) {
  //       uint64_t v = u + distance;
  //       MaxPrime prod ({{0, 0}});
  //       for (int i = u; i <= v; i++) prod *= MaxPrime(p.vector_factors_freq(i));
  //       if (prod.mult >= 2) {
  //         std::cout << "prime: " << prod.maxP << "^" << prod.mult << " = " << (long
  //         long)std::pow(prod.maxP, prod.mult) << std::endl; std::cout << distance << ' ' << u <<
  //         std::endl; break;
  //       }
  //     }
  //     u-=2;
  //   }
  // });

  timeit(
      []()
      {
        // choose p, then calculate p^2 -
      });
}
