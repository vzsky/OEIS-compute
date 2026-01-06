#include <cstdint>
#include <utils/ModInt.h>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <utils/Utils.h>
#include <vector>

template <uint64_t N> struct A389544
{
  using Int = PrimeInt;

  std::vector<Int> integerMap = std::vector<Int>(N + 1);
  std::vector<uint64_t> sequence;
  Prime<N> primeFactorizer{};

  A389544()
  {
    for (size_t i = 2; i <= N; i++)
      integerMap[i] = primeFactorizer.vector_factors_freq(i);
  }

  bool has_duplicate_product(const Int &targetProduct) const
  {
    Int candidate = 1;
    size_t l      = 0;

    for (size_t r = 0; r < sequence.size(); r++)
    {
      candidate *= integerMap[sequence[r]];

      while (l < r && !targetProduct.is_divisible_by(candidate))
      {
        candidate /= integerMap[sequence[l]];
        l++;
      }

      if (candidate == targetProduct)
        return true;
    }
    return false;
  }

  std::vector<uint64_t> get_sequence_until_N()
  {
    sequence   = {2};
    size_t ind = 1;

    for (uint64_t n = 3; n <= N; n++)
    { // adding n

      if (n % 1000 == 0)
        std::cout << "==== Progress: " << n << " / " << N << std::endl;

      if (has_duplicate_product(integerMap[n]))
        continue; // uninteresting skip

      std::vector<Int> productsToCheck = {};

      for (size_t trail = sequence.size(); trail-- > 0;)
      {
        if (primeFactorizer.is_prime(sequence[trail]))
          break;
        if (productsToCheck.empty())
          productsToCheck.push_back(integerMap[n] *
                                    integerMap[sequence[trail]]);
        else
          productsToCheck.push_back(productsToCheck.back() *
                                    integerMap[sequence[trail]]);
      }

      if (utils::par_all_of(begin(productsToCheck), end(productsToCheck),
                            [&](const Int &target)
                            { return !has_duplicate_product(target); }))
        sequence.push_back(n);
    }
    return sequence;
  }

  int is_interesting(uint64_t skippedElement) const
  {
    return !has_duplicate_product(integerMap[skippedElement]);
  }

  void print_interesting(std::vector<uint64_t> skipped)
  {
    sequence.clear();
    size_t j = 0;
    for (uint64_t i = 1; i <= N; i++)
    {
      if (i == skipped[j])
      {
        if (is_interesting(skipped[j]))
          std::cout << "interesting skip " << skipped[j] << std::endl;
        j++;
        continue;
      }
      sequence.push_back(i);
    }
  }
};
