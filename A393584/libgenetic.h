#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <unordered_map>
#include <utils/GeneticAlg.h>
#include <vector>

template <int N> class Gene
{
public:
  Gene(std::vector<int>&& v)
  {
    mMask = std::move(v);
    _calculate_score();
  }
  const std::vector<int>& get_mask() const { return mMask; }
  uint64_t get_fitness() const { return -mScore; }
  uint64_t get_score() const { return mScore; };
  uint64_t get_max_count() const { return mMaxCount; };

public:
  static Gene mutate(const Gene& parent, const genetic::SearchContext& ctx)
  {
    int num_swaps = N / 10;

    auto child_mask = parent.get_mask();
    std::vector<int> ones, zeros;
    for (int i = 0; i < child_mask.size(); ++i) (child_mask[i] ? ones : zeros).push_back(i);

    for (int i = 0; i < num_swaps; i++)
    { // we can swap 1 with 1 doing this but who cares
      std::swap(child_mask[ones[distN(ctx.rng)]], child_mask[zeros[distN(ctx.rng)]]);
    }
    return {std::move(child_mask)};
  }

  static Gene crossover(const Gene& p1, const Gene& p2, const genetic::SearchContext& ctx)
  {
    auto child_mask = p1.get_mask();
    std::vector<int> p1_ones_indices;
    std::vector<int> p2_ones_indices;

    for (int i = 0; i < p1.get_mask().size(); ++i)
    {
      if (p1.get_mask()[i] == 1 && p2.get_mask()[i] == 0)
        p1_ones_indices.push_back(i);
      else if (p1.get_mask()[i] == 0 && p2.get_mask()[i] == 1)
        p2_ones_indices.push_back(i);
    }

    std::shuffle(p1_ones_indices.begin(), p1_ones_indices.end(), ctx.rng);

    int num_to_swap = p1_ones_indices.size() / 2;
    for (int i = 0; i < num_to_swap; ++i)
    {
      child_mask[p1_ones_indices[i]] = 0;
      child_mask[p2_ones_indices[i]] = 1;
    }

    return {std::move(child_mask)};
  }

private:
  void _calculate_score()
  {
    std::vector<int> A, B;
    for (int i = 0; i < mMask.size(); i++)
    {
      if (mMask[i])
        A.push_back(i + 1);
      else
        B.push_back(i + 1);
    }

    mMaxCount = 0;

    // very unefficient. profiling do like above
    std::unordered_map<int, uint64_t> cnt;
    for (int a : A)
      for (int b : B) mMaxCount = std::max(mMaxCount, ++cnt[a - b]);

    std::unordered_map<uint64_t, uint64_t> cnt2;
    for (auto [x, c] : cnt) cnt2[c]++;

    mScore = 0;
    for (auto [d, c] : cnt2) mScore += d * d * c;
    mScore *= mMaxCount;
  }

  std::vector<int> mMask;
  uint64_t mScore, mMaxCount;
  static std::uniform_int_distribution<int> distN;
};

template <int N> std::uniform_int_distribution<int> Gene<N>::distN(0, N - 1);

static_assert(genetic::IsGene<Gene<0>>);
