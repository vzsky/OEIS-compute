#include <algorithm>
#include <array>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include <utils/Logging.hpp>
#include <utils/Prime.hpp>
#include <utils/Utils.hpp>

static constexpr std::array<uint64_t, 16> kQValues = {1, 2, 3, 5, 6, 7, 10, 14, 15, 21, 30, 35, 42, 70, 105, 210};
static constexpr std::array<int, 211> kQIndex = []() {
  std::array<int, 211> t{};
  t.fill(-1);
  for (int i = 0; i < 16; ++i) t[kQValues[i]] = i;
  return t;
}();

struct CandidateFilter
{
  void build(std::span<const uint64_t> factors)
  {
    mTailFactors.clear();
    mPeriod = 1;
    for (auto f : factors)
    {
      if (f <= 7) mPeriod *= f;
      else mTailFactors.push_back(f);
    }

    if (mPeriod == 1) { mSize = 1; mResidues[0] = 0; mGaps[0] = 1; return; }

    constexpr uint64_t kSmall[] = {2, 3, 5, 7};
    mSize = 0;
    for (uint64_t r = 0; r < mPeriod; ++r)
    {
      bool coprime = true;
      for (auto p : kSmall) if (mPeriod % p == 0 && r % p == 0) { coprime = false; break; }
      if (coprime) mResidues[mSize++] = r;
    }

    for (int i = 0; i + 1 < mSize; ++i) mGaps[i] = mResidues[i + 1] - mResidues[i];
    mGaps[mSize - 1] = mPeriod - mResidues[mSize - 1] + mResidues[0];
  }

  template<typename Pred>
  [[nodiscard]] uint64_t findFirstK(uint64_t& kFront, uint64_t p, uint64_t bound, Pred&& pred) const
  {
    std::array<uint64_t, 48> steps;
    for (int i = 0; i < mSize; ++i) steps[i] = mGaps[i] * p;

    uint64_t kMod = kFront % mPeriod;
    int wi = static_cast<int>(
      std::lower_bound(mResidues.begin(), mResidues.begin() + mSize, kMod) - mResidues.begin());
    uint64_t k = kFront - kMod;
    if (wi == mSize) { k += mPeriod; wi = 0; }
    k += mResidues[wi];

    // Phase 1: advance kFront monotonically through confirmed-used positions (pred=false).
    // Each wheel position is visited at most once here across the entire run (amortized O(1)).
    while (k * p < bound && !pred(k * p))
    {
      k += mGaps[wi];
      if (++wi == mSize) wi = 0;
    }
    kFront = k; // first possibly-unused position (or past bound)

    if (k * p >= bound) return UINT64_MAX;

    // Phase 2: scan from kFront for a coprimeTail-passing answer.
    // kFront is NOT updated here — tail factors change each step, so a coprimeTail failure
    // at k must not permanently hide that unused value from future queries.
    for (uint64_t cand = k * p; cand < bound;)
    {
      if (pred(cand) && coprimeTail(k)) return k;
      cand += steps[wi];
      k += mGaps[wi];
      if (++wi == mSize) wi = 0;
    }
    return UINT64_MAX;
  }

  [[nodiscard]] int periodQIndex() const { return kQIndex[mPeriod]; }

private:
  [[nodiscard]] bool coprimeTail(uint64_t k) const
  {
    for (auto f : mTailFactors) if (k % f == 0) return false;
    return true;
  }

  uint64_t mPeriod = 1;
  int mSize = 1;
  std::array<uint64_t, 48> mResidues{0};
  std::array<uint64_t, 48> mGaps{1};
  std::vector<uint64_t> mTailFactors;
};

template<uint64_t N, uint64_t SieveN = N * 5>
struct A098550
{
  [[nodiscard]] std::optional<uint64_t> next()
  {
    if (mN <= 3) return place(mN);
    if (mN > N) return std::nullopt;

    uint64_t best = UINT64_MAX;
    const int qi = mFilter.periodQIndex();

    for (auto p : mPrev2Factors)
    {
      uint64_t& kFront = mMinPQ.at(p)[qi];
      uint64_t k = mFilter.findFirstK(kFront, p, best,
                                      [&](uint64_t cand) { return cand >= mUsed.size() || !mUsed[cand]; });
      if (k != UINT64_MAX)
      {
        uint64_t cand = k * p;
        if (cand < best) best = cand;
      }
    }

    if (best == UINT64_MAX) return std::nullopt;
    return place(best);
  }

  [[nodiscard]] uint64_t index() const { return mN - 1; }

private:
  uint64_t place(uint64_t x)
  {
    mN++;
    if (x >= mUsed.size()) mUsed.resize(x * 2, 0);
    mUsed[x] = 1;
    mPrev2 = mPrev1;
    mPrev2Factors = std::move(mPrev1Factors);
    mPrev1 = x;
    mPrev1Factors = mSieve.distinct_factors(x);
    for (auto f : mPrev1Factors)
    {
      auto [it, inserted] = mMinPQ.try_emplace(f);
      if (inserted) it->second.fill(1);
    }
    mFilter.build(mPrev1Factors);
    return x;
  }

  PrimeSieve<SieveN> mSieve;
  uint64_t mN = 1;
  uint64_t mPrev2 = 0, mPrev1 = 0;
  std::vector<uint64_t> mPrev2Factors, mPrev1Factors;
  std::vector<char> mUsed = std::vector<char>(N * 4, 0);
  std::unordered_map<uint64_t, std::array<uint64_t, 16>> mMinPQ;
  CandidateFilter mFilter;
};

int main()
{
  utils::ScopeTimer _t("main");
  logging::Scope _l = logging::Env{}.module("main").logger(loggers::throttled_progress<5000>);
  constexpr uint64_t N = 500000;
  A098550<N> seq;
  while (auto x = seq.next())
    Log(LL::Info, "a($) = $"_f, seq.index(), *x);
}
