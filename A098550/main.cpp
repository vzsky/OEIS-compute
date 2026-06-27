#include <algorithm>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include <math/Basic.hpp>
#include <utils/JumpMap.hpp>
#include <utils/Logging.hpp>
#include <utils/Utils.hpp>
#include <utils/maya/Prime.hpp>

#include "lib.hpp"

static constexpr uint64_t kMidBound   = 1000000;
static constexpr uint64_t kSmallBound = 2;

struct CandidateFilter
{
  static constexpr std::array kSmallPrimes = maya::primes_upto<kSmallBound>();
  static constexpr std::array kPeriods     = []
  {
    constexpr int numPeriod = 1 << kSmallPrimes.size();

    uint64_t maxPeriod = 1;
    for (auto q : kSmallPrimes) maxPeriod *= q;

    std::array<uint64_t, numPeriod> divs{};
    int count = 0;
    for (uint64_t d = 1; d <= maxPeriod; ++d)
      if (maxPeriod % d == 0) divs[count++] = d;
    return divs;
  }();

  void build(const LazyFactors<kMidBound>& factors)
  {
    mMidFactors.clear();
    mBigFactor = 1;
    mPeriod    = 1;
    for (auto f : factors.primes)
    {
      if (f > kMidBound)
      {
        mBigFactor *= f;
        continue;
      }

      auto [it, inserted] = mMinPQ.try_emplace(f);
      if (inserted) it->second.fill(1);

      if (f <= kSmallBound)
        mPeriod *= f;
      else
        mMidFactors.push_back(f);
    }

    if (factors.remainder > 1)
    {
      mBigFactor *= factors.remainder;
    }

    mResidues.clear();
    if (mPeriod == 1)
    {
      mResidues.push_back(0);
      mGaps = {1};
      return;
    }

    for (uint64_t r = 0; r < mPeriod; ++r)
    {
      bool coprime = true;
      for (uint64_t p : kSmallPrimes)
        if (mPeriod % p == 0 && r % p == 0)
        {
          coprime = false;
          break;
        }
      if (coprime) mResidues.push_back(r);
    }

    mGaps.resize(mResidues.size());
    for (int i = 0; i + 1 < (int)mResidues.size(); ++i) mGaps[i] = mResidues[i + 1] - mResidues[i];
    mGaps.back() = mPeriod - mResidues.back() + mResidues[0];
  }

  [[nodiscard]] uint64_t findNext(const LazyFactors<kMidBound>& factors, auto&& pred)
  {
    uint64_t best = UINT64_MAX;
    for (auto p : factors.primes)
    {
      auto pred_k = [&](uint64_t k) { return pred(k * p); };
      if (p > kMidBound)
      {
        uint64_t kFront = 1;
        uint64_t k      = findFirstK(kFront, (best - 1) / p, pred_k);
        if (k != UINT64_MAX && k * p < best) best = k * p;
      }
      else
      {
        uint64_t& kFront = mMinPQ.at(p)[mPeriod];
        uint64_t k       = findFirstK(kFront, (best - 1) / p, pred_k);
        if (k != UINT64_MAX && k * p < best) best = k * p;
      }
    }

    for (auto p : factors.remaining_distinct_factors())
    {
      uint64_t kFront = 1;
      uint64_t k      = findFirstK(kFront, (best - 1) / p, [&](uint64_t k) { return pred(k * p); });
      if (k != UINT64_MAX && k * p < best) best = k * p;
    }

    return best;
  }

private:
  template <typename Pred> [[nodiscard]] uint64_t findFirstK(uint64_t& kFront, uint64_t bound_k, Pred&& pred)
  {
    const int sz = static_cast<int>(mResidues.size());

    uint64_t kMod = kFront % mPeriod;
    int wi = static_cast<int>(std::lower_bound(mResidues.begin(), mResidues.end(), kMod) - mResidues.begin());
    uint64_t k = kFront - kMod;
    if (wi == sz)
    {
      k += mPeriod;
      wi = 0;
    }
    k += mResidues[wi];

    // Phase 1: advance kFront monotonically through confirmed-used positions (pred=false).
    // Each wheel position is visited at most once here across the entire run (amortized O(1)).
    while (k <= bound_k && !pred(k))
    {
      k += mGaps[wi];
      if (++wi == sz) wi = 0;
    }
    kFront = k;

    if (k > bound_k) return UINT64_MAX;

    // Phase 2: scan from kFront for a coprimeTail-passing answer.
    // kFront is NOT updated here — tail factors change each step, so a coprimeTail failure
    // at k must not permanently hide that unused value from future queries.
    while (k <= bound_k)
    {
      if (pred(k) && coprimeTail(k)) return k;
      k += mGaps[wi];
      if (++wi == sz) wi = 0;
    }
    return UINT64_MAX;
  }

  [[nodiscard]] bool coprimeTail(uint64_t k) const
  {
    for (auto f : mMidFactors)
      if (k % f == 0) return false;
    if (math::gcd(k, mBigFactor) > 1) return false;
    return true;
  }

  uint64_t mPeriod = 1;
  std::vector<uint64_t> mResidues{0};
  std::vector<uint64_t> mGaps{1};
  std::vector<uint64_t> mMidFactors;
  uint64_t mBigFactor = 1;
  std::unordered_map<uint64_t, utils::JumpMap<kPeriods, uint64_t>> mMinPQ;
};

template <uint64_t N> struct A098550
{
  [[nodiscard]] std::optional<uint64_t> next()
  {
    if (mN <= 3) return place(mN);
    if (mN > N) return std::nullopt;

    auto isUnused = [&](uint64_t cand) { return cand >= mUsed.size() || !mUsed[cand]; };
    uint64_t best = mFilter.findNext(mPrev2Factors, isUnused);
    if (best == UINT64_MAX) return std::nullopt;
    return place(best);
  }

  [[nodiscard]] uint64_t index() const { return mN - 1; }

private:
  uint64_t place(uint64_t x)
  {
    mN++;
    if (x >= mUsed.size()) mUsed.resize(x * 2, false);
    mUsed[x]      = true;
    mPrev2        = mPrev1;
    mPrev2Factors = std::move(mPrev1Factors);
    mPrev1        = x;
    mPrev1Factors = mSieve.distinct_factors(x);
    mFilter.build(mPrev1Factors);
    return x;
  }

  CandidateFilter mFilter;
  LazyPrimeSieve<kMidBound> mSieve;
  LazyFactors<kMidBound> mPrev2Factors, mPrev1Factors;
  uint64_t mN     = 1;
  uint64_t mPrev2 = 0, mPrev1 = 0;
  std::vector<bool> mUsed = std::vector<bool>(N * 4, false);
};

int main()
{
  utils::ScopeTimer _t("main");
  logging::Scope _l    = logging::Env{}.module("main").logger(loggers::throttled_progress<100000>);
  constexpr uint64_t N = 50000000;
  A098550<N> seq;
  while (auto x = seq.next()) Log(LL::Info, "a($) = $"_f, seq.index(), *x);
}
