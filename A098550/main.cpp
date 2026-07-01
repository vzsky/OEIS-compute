#include <cmath>
#include <cstdint>
#include <numbers>
#include <span>
#include <vector>

#include "PrimeVector.hpp"
#include "RingBuffer.hpp"

#include <math/Basic.hpp>
#include <utils/Logging.hpp>
#include <utils/Prime.hpp>
#include <utils/Utils.hpp>
#include <utils/maya/Prime.hpp>

struct OptUint64
{
  OptUint64() = default;
  OptUint64(uint64_t v) : mVal(v) {}

  [[nodiscard]] explicit operator bool() const { return mVal != UINT64_MAX; }
  [[nodiscard]] uint64_t operator*() const { return mVal; }

private:
  uint64_t mVal = UINT64_MAX;
};

// checked in c3299eef49af2be3727e99042cf5d3ad73661fcc - took 16700 secs
// also conjectures (5,7,8,fixedPoint) hold
static constexpr uint64_t N = 10'000'000'000;
// the two numbers below only affect performance
static constexpr uint64_t kSieveBound = 100'000'000;
static constexpr uint64_t kFrontBound = 100'000'000;

// Conjectures 5, 7, 8, fixedPoint
struct ConjectureChecker
{
  void check(uint64_t n, uint64_t a)
  {
    mHistory.push(a);

    double conj7Bound = (std::numbers::pi / 2.0) * std::log(n) * n;
    if (n >= 2 && a > conj7Bound) reportBreach("Conj7 broken n=$: a=$, bound*n=$"_f, n, a, conj7Bound);

    if (a == n && n > 100) reportBreach("New fixed point: n=$"_f, n);

    if (maya::is_prime(a))
    {
      uint64_t nextExpected = next_prime(mLastPrime);
      if (a != nextExpected)
        reportBreach("Conj8 broken at n=$: a(n)=$ skipping over a prime $"_f, n, a, nextExpected);

      if (a > 97)
      {
        uint64_t prev2 = mHistory[2];
        uint64_t prev4 = mHistory[4];
        bool twoOk     = prev2 == 2 * a;
        bool fourOk    = prev4 % 2 == 0 && math::gcd(prev4 / 2, a) == 1;
        if (!twoOk || !fourOk)
          reportBreach("Conj5 broken at n=$: p=$, a(n-2)=$, a(n-4)=$"_f, n, a, prev2, prev4);
      }

      mLastPrime = a;
    }
  }

private:
  static uint64_t next_prime(uint64_t p)
  {
    if (p < 2) return 2;
    if (p == 2) return 3;
    uint64_t c = p + 2;
    while (!maya::is_prime(c)) c += 2;
    return c;
  }

  template <typename... Args> static void reportBreach(Args&&... args)
  {
    logging::Scope _l = logging::Env{}.module("conj").logger(loggers::normal);
    Log(LL::Warn, std::forward<Args>(args)...);
  }

  RingBuffer<8> mHistory;
  uint64_t mLastPrime = 0;
};

struct CandidateFilter
{
  void mustCoprimeTo(std::span<const uint64_t> factors)
  {
    mPrimesMustntDivide = factors;
    mMustBeOdd          = !factors.empty() && factors[0] == 2;
  }

  void markUsed(uint64_t x)
  {
    while (x >= mUsed.size()) mUsed.resize(x * 2, false);
    mUsed[x] = true;
  }

  [[nodiscard]] OptUint64 findNotCoprimeWith(std::span<const uint64_t> factors)
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
        if (!isUsed(k * p) &&
            std::ranges::none_of(mPrimesMustntDivide, [k](uint64_t f) { return k % f == 0; }))
        {
          best = k * p;
          break;
        }
        k += period;
      }
    }
    return best;
  }

  [[nodiscard]] bool isUsed(uint64_t x) const { return x < mUsed.size() && mUsed[x]; }

  bool mMustBeOdd = false;
  std::span<const uint64_t> mPrimesMustntDivide;
  PrimeVector<kFrontBound, std::array<uint64_t, 2>> mPrimeFronts{std::array<uint64_t, 2>{1, 1}};
  std::vector<bool> mUsed = std::vector<bool>(N, false);
};

template <uint64_t N> struct A098550
{
  OptUint64 next()
  {
    if (mNextIndex <= 3) return place(mNextIndex);
    if (mNextIndex > N) return {};

    auto best = mFilter.findNotCoprimeWith(mPrev2Factors);
    if (!best) return {};
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
  PrimeSieve<kSieveBound> mSieve;
  std::vector<uint64_t> mPrev2Factors, mPrev1Factors;
  uint64_t mPrev2 = 0, mPrev1 = 0;
  uint64_t mNextIndex = 1;
};

int main()
{
  utils::ScopeTimer _t("main");
  logging::Scope _l = logging::Env{}.module("main").logger(loggers::throttled_progress<100000>);
  A098550<N> seq;
  ConjectureChecker checker;
  while (auto x = seq.next())
  {
    Log(LL::Info, "a($) = $"_f, seq.index(), *x);
    checker.check(seq.index(), *x);
  }
}
