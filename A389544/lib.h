#include <algorithm>
#include <cstdint>
#include <unordered_set>
#include <utils/LogInt.h>
#include <utils/ModInt.h>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <utils/Utils.h>
#include <vector>

template <uint32_t N, uint32_t CacheLim> struct A389544
{
  using Int = PrimeInt;
  static_assert(CacheLim > N);
  static_assert(CacheLim < std::numeric_limits<uint32_t>::max());

private:
  virtual void _skip(uint64_t n) {}
  virtual void _not_skip(uint64_t n) {}

  void skip(uint64_t n) { _skip(n); }

  void not_skip(uint64_t n)
  {
    _not_skip(n);
    uint64_t cand = n;
    for (size_t trail = sequence.size(); trail-- > 0;)
    {
      cand *= sequence[trail];
      if (cand >= CacheLim) break;
      consecCache.insert(cand);
    }
    _not_skip(n);
    sequence.push_back(n);
  }

public:
  A389544()
  {
    integerMap.resize(N);
    for (size_t i = 2; i <= N; i++) integerMap[i] = primeFactorizer.vector_factors_freq(i);
    consecCache = {};
    sequence.reserve(N);
    sequence.push_back(2);
  }

  bool has_duplicate_product_cache(const uint64_t& targetProduct) const
  {
    assert(targetProduct < CacheLim);
    return consecCache.contains(targetProduct);
  }

  bool product_is_lower_bound(size_t startInd, size_t endInd, const Int& targetProduct) const
  {
    { // size-wise
      LogInt target    = targetProduct;
      LogInt candidate = 1;
      for (size_t i = startInd; i < endInd; i++)
      {
        candidate *= sequence[i];
        if (target.surely_lt(candidate))
        {
          stats.size_wise++;
          return false;
        }
      }
    }
    { // divisibility-wise
      Int candidate{1};
      for (size_t i = startInd; i < endInd; i++)
      {
        candidate *= integerMap[sequence[i]];
        if (!targetProduct.is_divisible_by(candidate))
        {
          stats.div_wise++;
          return false;
        }
      }
    }
    return true;
  }

  inline bool optimization_1(const Int& targetProduct) const
  {
    const auto& factors = targetProduct.factors();
    for (auto it = factors.rbegin(); it != factors.rend(); ++it)
    {
      const auto& [p, m] = *it;
      if (p == 2 || m <= 1) continue;
      if (m >= p) continue;

      // if multplicity of p is m, we at least need p, .., m * p
      auto startInd = std::lower_bound(sequence.begin(), sequence.end(), p) - sequence.begin();
      auto endInd   = std::upper_bound(sequence.begin(), sequence.end(), m * p) - sequence.begin();
      if (!product_is_lower_bound(startInd, endInd, targetProduct))
      {
        stats.opt1++;
        return true;
      }
    }
    return false;
  }

  inline bool optimization_2(const Int& targetProduct) const
  {
    const auto& factors         = targetProduct.factors();
    const uint64_t largestPrime = factors.back().first;
    const size_t largestPrimeIndex =
        std::lower_bound(sequence.begin(), sequence.end(), largestPrime) - sequence.begin();

    if (factors.size() == 1) return false;

    uint64_t endForward  = largestPrime;
    uint64_t endBackward = largestPrime;
    for (int k = 2; k <= factors.size(); k++)
    {
      const auto nextPrime = factors[factors.size() - k].first;
      endForward           = std::max(endForward, largestPrime - (largestPrime % nextPrime) + nextPrime);
      endBackward          = std::min(endBackward, largestPrime - (largestPrime % nextPrime));
    }

    size_t indForward  = std::upper_bound(sequence.begin(), sequence.end(), endForward) - sequence.begin();
    size_t indBackward = std::lower_bound(sequence.begin(), sequence.end(), endBackward) - sequence.begin();

    bool failedForward  = !product_is_lower_bound(largestPrimeIndex, indForward, targetProduct);
    bool failedBackward = !product_is_lower_bound(indBackward, largestPrimeIndex + 1, targetProduct);

    if (failedForward && failedBackward)
    {
      stats.opt2++;
      return true;
    }
    return false;
  }

  inline bool optimization_3(const Int& targetProduct, const uint64_t multipliedTerms) const
  {
    // if there is a consecutive product = acc, it must has > (what constructed acc) terms;
    // (what constructed acc ~ log_n(acc))
    // since all numbers are less than n
    uint64_t minimumTerms = multipliedTerms + 1;
    const auto& factors   = targetProduct.factors();

    const auto largestPrime = factors.back().first;
    const size_t largestPrimeIndex =
        std::lower_bound(sequence.begin(), sequence.end(), largestPrime) - sequence.begin();

    if (largestPrimeIndex < minimumTerms) return false;

    if (!product_is_lower_bound(largestPrimeIndex - minimumTerms + 1, largestPrimeIndex + 1, targetProduct))
    {
      stats.opt3++;
      return true;
    }

    return false;
  }

  bool duplicate_product_impossible(const Int& targetProduct, const uint64_t multipliedTerms) const
  {
    if (optimization_3(targetProduct, multipliedTerms)) return true;
    if (optimization_1(targetProduct)) return true;
    if (optimization_2(targetProduct)) return true;
    return false;
  }

  bool has_duplicate_product_loop(const Int& targetProduct) const
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

      if (candidate == targetProduct) return true;
    }
    return false;
  }

  void try_add_number(uint64_t n)
  {
    if (primeFactorizer.is_prime(n)) return not_skip(n);

    stats.cached++;
    if (has_duplicate_product_cache(n)) [[unlikely]]
      return skip(n);

    mCurrentProductsToCheck.clear();
    Int acc            = integerMap[n];
    uint64_t acc_small = n;
    for (int trail = sequence.size() - 1; trail >= 0; trail--)
    {
      if (primeFactorizer.is_prime(sequence[trail])) break;
      acc *= integerMap[sequence[trail]];

      acc_small *= sequence[trail];
      if (acc_small != 0 && acc_small < CacheLim)
      {
        stats.cached++;
        if (has_duplicate_product_cache(acc_small)) [[unlikely]]
          return skip(n);
        continue;
      }
      else
        acc_small = 0;

      const auto multipliedTerms = sequence.size() - trail + 1;
      if (duplicate_product_impossible(acc, multipliedTerms)) continue;
      mCurrentProductsToCheck.push_back(acc);
      // std::cout << "need to loop " << n << ' ' << multipliedTerms << " -> " << acc << std::endl;
    }

    stats.loop += mCurrentProductsToCheck.size();
    if (std::all_of(begin(mCurrentProductsToCheck), end(mCurrentProductsToCheck),
                    [&](const Int& target) { return !has_duplicate_product_loop(target); }))
      return not_skip(n);

    return skip(n);
  }

  const std::vector<uint64_t>& get_sequence_until_N()
  {
    uint64_t start = sequence.back() + 1;
    for (uint64_t n = start; n <= N; n++)
    {
      if (n % 500000 == 0)
      {
        std::cout << "==== Progress: " << n << " / " << N << std::endl;
        stats.print();
      }
      try_add_number(n);
    }
    return sequence;
  }

  bool is_interesting(uint64_t skippedElement) const
  {
    return !has_duplicate_product_loop(integerMap[skippedElement]);
  }

private:
  std::vector<Int> mCurrentProductsToCheck{};

public:
  std::vector<Int> integerMap              = std::vector<Int>(N + 1);
  std::unordered_set<uint64_t> consecCache = std::unordered_set<uint64_t>();
  std::vector<uint64_t> sequence;
  Prime<N> primeFactorizer{};

  mutable struct Stats
  {
    uint64_t loop   = 0;
    uint64_t cached = 0;
    uint64_t opt1   = 0;
    uint64_t opt2   = 0;
    uint64_t opt3   = 0;
    uint64_t opt4   = 0;

    uint64_t size_wise = 0;
    uint64_t div_wise  = 0;

    void print() const
    {
      std::cout << "Cached " << cached << std::endl;
      std::cout << "Loop " << loop << std::endl;
      std::cout << "Opt1 " << opt1 << std::endl;
      std::cout << "Opt2 " << opt2 << std::endl;
      std::cout << "Opt3 " << opt3 << std::endl;
      std::cout << "Opt4 " << opt4 << std::endl;
      std::cout << "---------------------------\n";
      std::cout << "Size " << size_wise << std::endl;
      std::cout << "Div " << div_wise << std::endl;
    }
  } stats;
};
