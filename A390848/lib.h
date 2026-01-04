#include <utils/ModInt.h>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <utils/Utils.h>
#include <vector>

template <int N> struct A389544
{

  using Int = PrimeInt;

  std::vector<Int> integerMap = std::vector<Int>(N + 1);
  std::vector<int> sequence = {2};
  Prime<N> primeFactorizer{};

  A389544()
  {
    for (int i = 2; i <= N; i++)
      integerMap[i] = primeFactorizer.vector_factors_freq(i);
  }

  void add_to_seqeunce(int v)
  {
    sequence.push_back(v);
  }

  bool has_duplicate_product(const Int& targetProduct) const
  {
    Int candidate = 1;
    size_t l = 0;

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

  std::vector<int> get_sequence_until_N()
  {
    size_t ind = 1;

    for (uint64_t n = 3; n <= N; n++)
    { // adding n

      if (n % 1000 == 0)
      {
        std::cout << "========= " << n << " ==========" << std::endl;
      }

      if (has_duplicate_product(integerMap[n]))
      {
        // uninteresting skip
        std::cout << ind++ << ' ' << n << std::endl;
        continue;
      }

      std::vector<Int> productsToCheck = {};

      for (size_t trail = sequence.size(); trail-- > 0;)
      {
        if (primeFactorizer.is_prime(sequence[trail]))
          break;
        if (productsToCheck.empty())
          productsToCheck.push_back(integerMap[n] * integerMap[sequence[trail]]);
        else
          productsToCheck.push_back(productsToCheck.back() * integerMap[sequence[trail]]);
      }

      if (utils::par_all_of(begin(productsToCheck), end(productsToCheck),
                            [&](const Int& target) { return !has_duplicate_product(target); }))
      {
        add_to_seqeunce(n);
      }
      else
      {
        std::cout << ind++ << ' ' << n << std::endl;
      }
    }

    return sequence;
  }
};
