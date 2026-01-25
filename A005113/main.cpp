#include "lib.h"
#include <cstdint>
#include <iostream>
#include <set>
#include <unordered_map>
#include <utils/Prime.h>

std::set<uint64_t> primes_in_class[20];

constexpr size_t Prime_N = 1'00'000'000;
Prime<Prime_N> primeFact;

int get_prime_class(size_t p)
{
  static std::unordered_map<size_t, int> cache;

  if (p <= 3) return 0;
  if (cache.count(p)) return cache[p];

  int max_factor_class = 0;
  for (const auto& [q, freq] : primeFact.vector_factors_freq(p + 1))
  {
    max_factor_class = std::max(max_factor_class, get_prime_class(q));
  }

  return cache[p] = 1 + max_factor_class;
}

void ensure_complete_until(int cls, uint64_t target)
{
  // std::cout << "ensuring " << cls << " until " << target << std::endl;
  if (target < Prime_N) return;
  ensure_complete_until(cls - 1, target / 2 + 1);

  for (uint64_t p : primes_in_class[cls - 1])
  {
    if (p > target / 2 + 1) break;

    uint64_t i_start = (Prime_N + p) / p;
    if (i_start & 1) i_start++;
    for (uint64_t i = i_start;; i += 2)
    {
      uint64_t candidate = i * p - 1;
      if (candidate > target) break;

      if (primeFact.is_prime(candidate))
      {
        primes_in_class[cls].insert(candidate);
      }
    }
  }
  // std::cout << "ensured " << cls << " until " << target << std::endl;
}

int main()
{
  int target_class = 0;
  {
    std::cout << "Class\t| Smallest Prime" << std::endl;
    std::cout << "------------------------" << std::endl;
    for (size_t p : primeFact.all_primes())
    {
      int c = get_prime_class(p);
      if (c == target_class)
      {
        std::cout << "  " << c << "\t| " << p << std::endl;
        target_class++;
      }
      primes_in_class[c].insert(p);
    }
  }

  ensure_complete_until(14, 1704961513);
  std::cout << "  13" << "\t| " << *primes_in_class[13].begin() << std::endl;
  std::cout << "  14" << "\t| " << *primes_in_class[14].begin() << std::endl;

  return 0;
}
