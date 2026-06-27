#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "lib.hpp"
#include <math/Basic.hpp>
#include <utils/Logging.hpp>
#include <utils/Utils.hpp>

static constexpr uint64_t N = 100'000'000;
// the two numbers below only affect performance
static constexpr uint64_t kSieveBound = 10'000'000;
static constexpr uint64_t kFrontBound = 1'000'000;

struct CandidateFilter
{
  void mustCoprimeTo(std::span<const uint64_t> factors)
  {
    mPrimesSpan = factors;
    mMustBeOdd  = !factors.empty() && factors[0] == 2;
  }

  void markUsed(uint64_t x)
  {
    if (x >= mUsed.size()) mUsed.resize(x * 2, false);
    mUsed[x] = true;
  }

  [[nodiscard]] std::optional<uint64_t> findNotCoprimeWith(std::span<const uint64_t> factors)
  {
    uint64_t best   = UINT64_MAX;
    uint64_t period = mMustBeOdd + 1;
    for (auto p : factors)
    {
      uint64_t bound = (best - 1) / p;
      uint64_t k     = 1;
      if (p < kFrontBound)
      {
        uint64_t& kFront = mPrimeFronts[p][mMustBeOdd];
        k                = kFront | mMustBeOdd;
        while (k <= bound && isUsed(k * p)) k += period;
        kFront = k;
      }
      while (k <= bound)
      {
        if (!isUsed(k * p) && std::ranges::none_of(mPrimesSpan, [k](uint64_t f) { return k % f == 0; }))
        {
          best = k * p;
          break;
        }
        k += period;
      }
    }
    return best == UINT64_MAX ? std::nullopt : std::optional{best};
  }

  [[nodiscard]] bool isUsed(uint64_t x) const { return x < mUsed.size() && mUsed[x]; }

  bool mMustBeOdd;
  std::span<const uint64_t> mPrimesSpan;
  PrimeVector<kFrontBound, std::array<uint64_t, 2>> mPrimeFronts{std::array<uint64_t, 2>{1, 1}};
  std::vector<bool> mUsed = std::vector<bool>(N, false);
};

template <uint64_t N> struct A098550
{
  std::optional<uint64_t> next()
  {
    if (mNextIndex <= 3) return place(mNextIndex);
    if (mNextIndex > N) return std::nullopt;

    auto best = mFilter.findNotCoprimeWith(mPrev2Factors);
    if (!best) return std::nullopt;
    return place(*best);
  }

  [[nodiscard]] uint64_t index() const { return mNextIndex - 1; }

private:
  uint64_t place(uint64_t x)
  {
    mNextIndex++;
    mPrev2        = mPrev1;
    mPrev1        = x;
    mPrev2Factors = std::move(mPrev1Factors);
    mPrev1Factors = mSieve.distinct_factors(x);
    mFilter.markUsed(x);
    mFilter.mustCoprimeTo(mPrev1Factors);
    return x;
  }

  CandidateFilter mFilter;
  FastPrimeSieve<kSieveBound> mSieve;
  std::vector<uint64_t> mPrev2Factors, mPrev1Factors;
  uint64_t mPrev2 = 0, mPrev1 = 0;
  uint64_t mNextIndex = 1;
};

int main()
{
  utils::ScopeTimer _t("main");
  logging::Scope _l = logging::Env{}.module("main").logger(loggers::throttled_progress<100000>);
  A098550<N> seq;
  while (auto x = seq.next()) Log(LL::Info, "a($) = $"_f, seq.index(), *x);
}
