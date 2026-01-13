#include <algorithm>
#include <cstdint>
#include <unordered_set>
#include <utils/ComplementNonnVector.h>
#include <utils/LogInt.h>
#include <utils/ModInt.h>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>
#include <utils/Utils.h>
#include <vector>

using namespace complementNonnVector;

template <uint32_t N, uint32_t CacheLim> struct A389544
{
  using Int = PrimeInt;
  static_assert(CacheLim > N);
  static_assert(CacheLim < std::numeric_limits<uint32_t>::max());

private:
  virtual void _skip(uint64_t n) {}
  virtual void _not_skip(uint64_t n) {}

  void skip(uint64_t n)
  {
    seq.add_skip(n);
    _skip(n);
  }

  void not_skip(uint64_t n)
  {
    uint64_t cand = n;
    for (auto it = seq.it_at(seqSize - 1); it.idx() >= 0; --it)
    {
      cand *= *it;
      if (cand >= CacheLim) break;
      consecCache.insert(cand);
    }

    _not_skip(n);
    seqSize++;
  }

  void make_conseccache()
  {
    uint64_t prod = 1;
    auto l        = seq.it_at(0);

    for (auto r = seq.it_at(0); r.idx() < seqSize; ++r)
    {
      prod *= *r;

      while (l != r && prod >= CacheLim)
      {
        prod /= *l;
        ++l;
      }

      consecCache.insert(prod);
    }
  }

public:
  A389544()
  {
    consecCache = {};

    // seq = {2}
    seq.add_skip(0);
    seq.add_skip(1);
    seqSize = 1;
  }

  Int toInteger(uint64_t n) const { return primeFactorizer.vector_factors_freq(n); }

  bool has_duplicate_product_cache(const uint64_t& targetProduct) const
  {
    assert(targetProduct < CacheLim);
    return consecCache.contains(targetProduct);
  }

  bool product_is_size_lower_bound(size_t startInd, size_t endInd, const Int& targetProduct) const
  {
    return product_is_size_lower_bound(seq.it_at(startInd), endInd, targetProduct);
  }

  bool product_is_div_lower_bound(size_t startInd, size_t endInd, const Int& targetProduct) const
  {
    return product_is_div_lower_bound(seq.it_at(startInd), endInd, targetProduct);
  }

  bool product_is_div_lower_bound(Vector::iterator startIt, size_t endInd, const Int& targetProduct) const
  {
    Int candidate{1};
    for (auto it = startIt; it.idx() < endInd; ++it)
    {
      candidate *= toInteger(*it);
      if (!targetProduct.is_divisible_by(candidate)) return false;
    }
    return true;
  }

  bool product_is_size_lower_bound(Vector::iterator startIt, size_t endInd, const Int& targetProduct) const
  {
    LogInt target    = targetProduct;
    LogInt candidate = 1;

    for (auto it = startIt; it.idx() < endInd; ++it)
    {
      candidate *= *it;
      if (target.surely_lt(candidate)) return false;
    }
    return true;
  }

  bool optimization_1(const Int& targetProduct) const
  {
    const auto& factors = targetProduct.factors();
    for (auto it = factors.rbegin(); it != factors.rend(); ++it)
    {
      const auto& [p, m] = *it;
      if (p == 2 || m <= 1) continue;
      if (m >= p) continue;

      // if multplicity of p is m, we at least need p, .., m * p
      const Vector::iterator startIt = seq.lower_bound(p);
      const size_t endInd            = seq.upper_bound(m * p).idx();
      if (!product_is_size_lower_bound(startIt, endInd, targetProduct))
      {
        stats.opt1_size++;
        return true;
      }
      if (!product_is_div_lower_bound(startIt, endInd, targetProduct))
      {
        stats.opt1_div++;
        return true;
      }
    }
    return false;
  }

  bool optimization_2(const Int& targetProduct) const
  {
    const auto& factors            = targetProduct.factors();
    const uint64_t largestPrime    = factors.back().first;
    const size_t largestPrimeIndex = seq.lower_bound(largestPrime).idx();

    if (factors.size() == 1) return false;

    uint64_t endForward  = largestPrime;
    uint64_t endBackward = largestPrime;
    for (int k = 2; k <= factors.size(); k++)
    {
      const auto nextPrime = factors[factors.size() - k].first;
      endForward           = std::max(endForward, largestPrime - (largestPrime % nextPrime) + nextPrime);
      endBackward          = std::min(endBackward, largestPrime - (largestPrime % nextPrime));
    }

    size_t indForward  = seq.upper_bound(endForward).idx();
    size_t indBackward = seq.lower_bound(endBackward).idx();

    bool forwardSizeBound  = product_is_size_lower_bound(largestPrimeIndex, indForward, targetProduct);
    bool backwardSizeBound = product_is_size_lower_bound(indBackward, largestPrimeIndex + 1, targetProduct);

    if (!forwardSizeBound && !backwardSizeBound)
    {
      stats.opt2_size++;
      return true;
    }

    bool forwardDivBound = product_is_div_lower_bound(largestPrimeIndex, indForward, targetProduct);
    if (forwardDivBound) return false;
    bool backwardDivBound = product_is_div_lower_bound(indBackward, largestPrimeIndex + 1, targetProduct);
    if (backwardDivBound) return false;

    stats.opt2_div++;
    return true;
  }

  bool optimization_3(const Int& targetProduct, const uint64_t multipliedTerms) const
  {
    // if there is a consecutive product = acc, it must has > (what constructed acc) terms;
    // (what constructed acc ~ log_n(acc))
    // since all numbers are less than n
    uint64_t minimumTerms = multipliedTerms + 1;
    const auto& factors   = targetProduct.factors();

    const auto largestPrime        = factors.back().first;
    const size_t largestPrimeIndex = seq.lower_bound(largestPrime).idx();

    if (largestPrimeIndex < minimumTerms) return false;

    if (!product_is_size_lower_bound(largestPrimeIndex - minimumTerms + 1, largestPrimeIndex + 1,
                                     targetProduct))
    {
      stats.opt3_size++;
      return true;
    }

    if (!product_is_div_lower_bound(largestPrimeIndex, largestPrimeIndex + minimumTerms, targetProduct) &&
        !product_is_div_lower_bound(largestPrimeIndex - minimumTerms + 1, largestPrimeIndex + 1,
                                    targetProduct))
    {
      stats.opt3_div++;
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

    for (size_t r = 0; r < seqSize; r++)
    {
      candidate *= toInteger(seq[r]);

      while (l < r && !targetProduct.is_divisible_by(candidate))
      {
        candidate /= toInteger(seq[l]);
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
    Int acc            = toInteger(n);
    uint64_t acc_small = n;

    for (auto it = seq.it_at(seqSize - 1); it.idx() >= 0; --it)
    {
      if (primeFactorizer.is_prime(*it)) break;
      acc *= toInteger(*it);

      acc_small *= *it;
      if (acc_small != 0 && acc_small < CacheLim)
      {
        stats.cached++;
        if (has_duplicate_product_cache(acc_small)) [[unlikely]]
          return skip(n);
        continue;
      }
      else
        acc_small = 0;

      const auto multipliedTerms = seqSize - it.idx() + 1;
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

  void get_sequence_until_N()
  {
    uint64_t start = seq[seqSize - 1] + 1;
    for (uint64_t n = start; n <= N; n++)
    {
      if (n % 1000000 == 0)
      {
        std::cout << "==== Progress: " << n << " / " << N << std::endl;
        stats.print();
      }
      try_add_number(n);
    }
  }

  void read_skipped(const std::vector<uint64_t> skipped)
  {
    for (auto n : skipped) seq.add_skip(n);
    seqSize = skipped.back() + 1 - skipped.size();
    make_conseccache();
    std::cout << "read skipped" << std::endl;
  }

  bool is_interesting(uint64_t skippedElement) const
  {
    return !has_duplicate_product_loop(toInteger(skippedElement));
  }

private:
  std::vector<Int> mCurrentProductsToCheck{};

public:
  std::unordered_set<uint64_t> consecCache = std::unordered_set<uint64_t>();
  Prime<N> primeFactorizer{};

  Vector seq{};
  size_t seqSize = 0;

  mutable struct Stats
  {
    uint64_t loop      = 0;
    uint64_t cached    = 0;
    uint64_t opt1_size = 0;
    uint64_t opt1_div  = 0;
    uint64_t opt2_size = 0;
    uint64_t opt2_div  = 0;
    uint64_t opt3_size = 0;
    uint64_t opt3_div  = 0;

    void print() const
    {
      std::cout << "Cached " << cached << std::endl;
      std::cout << "Loop " << loop << std::endl;
      std::cout << "Opt1 size " << opt1_size << std::endl;
      std::cout << "Opt1 div " << opt1_div << std::endl;
      std::cout << "Opt2 size " << opt2_size << std::endl;
      std::cout << "Opt2 div " << opt2_div << std::endl;
      std::cout << "Opt3 size " << opt3_size << std::endl;
      std::cout << "Opt3 div " << opt3_div << std::endl;
    }
  } stats;
};
