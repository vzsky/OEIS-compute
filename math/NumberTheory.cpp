#include <cassert>
#include <math/Basic.h>
#include <math/NumberTheory.h>
#include <utils/Prime.h>

using namespace math;

namespace numtheory
{

int legendre_symbol(int a, int p)
{
  assert(p > 2 && Prime<3>{}.is_prime(p));
  a %= p;
  if (a < 0) a += p;
  if (a == 0) return 0;
  int r = pow(a, (p - 1) / 2, p);
  if (r == 1) return 1;
  if (r == p - 1) return -1;
  return 0;
}

int jacobi_symbol(int a, int n)
{
  assert(n > 0 && (n % 2 == 1));
  a %= n;
  if (a < 0) a += n;
  if (a == 0) return 0;

  int result = 1;

  while (a != 0)
  {
    while (a % 2 == 0)
    {
      a /= 2;
      int r = n % 8;
      if (r == 3 || r == 5) result = -result;
    }

    std::swap(a, n);

    if (a % 4 == 3 && n % 4 == 3) result = -result;

    a %= n;
  }

  return (n == 1) ? result : 0;
}

} // namespace numtheory
