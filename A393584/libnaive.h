#pragma once

#include <limits>
#include <utils/MoreMath.h>
#include <vector>

namespace common
{

inline int max_diff_count(const std::vector<int>& mask)
{
  int n      = mask.size();
  int offset = n;

  static std::vector<int> cnt;
  cnt.assign(2 * n + 1, 0);

  const int* m = mask.data();
  int curMax   = 0;

  for (int i = 0; i < n; ++i)
    if (m[i])
    {
      for (int j = 0; j < n; ++j)
        if (!m[j])
        {
          curMax = std::max(curMax, ++cnt[i - j + offset]);
        }
    }

  return curMax;
}

template <int N> std::vector<int> nth_binary_permutation(uint64_t idx)
{
  int cnt[2];

  cnt[0] = N, cnt[1] = N;

  std::vector<int> res;
  const auto put = [&](int i)
  {
    res.push_back(i);
    cnt[i]--;
  };

  for (int i = 0; i < 2 * N; i++)
  {
    if (!cnt[0] || !cnt[1]) break;

    // double otherwise need bigint
    uint64_t choose = math::nCk<double>(cnt[0] + cnt[1] - 1, cnt[0] - 1);
    put(idx >= choose);
    if (idx >= choose) idx -= choose;
  }

  while (cnt[0]--) put(0);
  while (cnt[1]--) put(1);

  return res;
}

} // namespace common

namespace naive
{

template <int N> int upperbound_answer(int l, int r)
{
  int m     = 2 * N;
  auto mask = common::nth_binary_permutation<N>(l);

  int best = std::numeric_limits<int>::max();
  std::vector<int> save;
  int cnt = l;
  do
  {
    if (mask[0] == 1) break; // second half
    if (l && r && r - l > 0 && cnt++ > r - l) break;

    auto curMax = common::max_diff_count(mask);
    if (curMax < best)
    {
      save = mask;
      best = curMax;
    }
    // std::cout << curMax << ' ';
  } while (next_permutation(mask.begin(), mask.end()));

  // std::cout << "> "; for (auto x : save) std::cout << x << ' '; std::cout << std::endl;
  return best;
}

template <int N> int exact_answer() { return upperbound_answer<N>(0, 0); }

} // namespace naive
