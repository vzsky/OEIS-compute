#include <cstdint>
#include <utils/Prime.hpp>
#include <utils/PrimeInt.hpp>
#include <utils/Utils.hpp>

PrimeSieve<5000000> p;

// prime: 3^2 = 9           -> 1 8
// prime: 7^2 = 49          -> 2 48
// prime: 41^2 = 1681       -> 3 1680
// prime: 277^2 = 76729     -> 4 76725
// prime: 577^2 = 332929    -> 5 332925
// prime: 2131^2 = 4541161  -> 6 4541155
// prime: 2131^2 = 4541161  -> 7 4541154
// prime: 8663^2 = 75047569 -> 8 75047565
// it seems that all answers so far are (p^2 - k) where p is the largest prime
// divisor

struct MaxPrime
{
  size_t maxP, mult;

  MaxPrime(std::vector<std::pair<size_t, size_t>> factors)
  {
    maxP = factors.back().first;
    mult = factors.back().second;
  }

  const MaxPrime& operator*=(const MaxPrime& other)
  {
    if (other.maxP == maxP) mult += other.mult;
    if (other.maxP > maxP)
    {
      maxP = other.maxP;
      mult = other.mult;
    }
    return *this;
  }
};

void print_from_start(size_t start, size_t width)
{
  for (size_t i = start; i <= start + width; i++)
    Log(LL::Info, "$ == $"_f, i, PrimeInt(p.vector_factors_freq(i)));
}

bool is_valid_start(size_t start, size_t width)
{
  MaxPrime prod({{0, 0}});
  for (size_t i = start; i <= start + width; i++) prod *= MaxPrime(p.vector_factors_freq(i));

  return prod.mult >= 2;
}

void find_short_sequence()
{
  uint64_t u = 1;
  for (int distance = 1; distance < 10; distance++)
  {
    while (u++)
    {
      uint64_t v = u + distance;
      MaxPrime prod({{0, 0}});
      for (uint64_t i = u; i <= v; i++) prod *= MaxPrime(p.vector_factors_freq(i));
      if (prod.mult >= 2)
      {
        Log(LL::Info, "prime: $^$ = $"_f, prod.maxP, prod.mult, (long long)std::pow(prod.maxP, prod.mult));
        Log(LL::Info, distance, u);
        break;
      }
    }
    u -= 2;
  }
}

void find_upper_bound(size_t width)
{
  for (auto prime : p.all_primes())
  {
    if (prime < width) continue;

    uint64_t psqr = prime * prime;

    for (uint64_t start = psqr - width; start <= psqr; ++start)
    {
      bool found = true;
      for (size_t i = 0; i <= width; ++i)
      {
        if (p.factors(start + i).back() > prime)
        {
          found = false;
          break;
        }
      }

      if (found)
      {
        Log(LL::Info, "Prime $"_f, prime);
        print_from_start(start, width);
        if (is_valid_start(start, width)) Log(LL::Info, width, start);
        return; // or width ++ to continue the search
      }
    }
  }
}

void upper_bound_cert()
{
  {
    size_t start = 4'928'180'396;
    size_t width = 9;
    // print_from_start(start, width);
    if (is_valid_start(start, width) && !is_valid_start(start, width + 1))
      Log(LL::Info, "$ confirmed"_f, width);
  }
  {
    size_t start = 40'533'366'231;
    size_t width = 10;
    // print_from_start(start, width);
    if (is_valid_start(start, width) && !is_valid_start(start, width + 1))
      Log(LL::Info, "$ confirmed"_f, width);
  }
  {
    size_t start = 111'460'496'439;
    size_t width = 11;
    // print_from_start(start, width);
    if (is_valid_start(start, width) && !is_valid_start(start, width + 1))
      Log(LL::Info, "$ confirmed"_f, width);
  }
  {
    size_t start = 436'502'026'483;
    size_t width = 12;
    // print_from_start(start, width);
    if (is_valid_start(start, width) && !is_valid_start(start, width + 1))
      Log(LL::Info, "$ confirmed"_f, width);
  }
}

int main()
{
  upper_bound_cert();
  return 0;
}
