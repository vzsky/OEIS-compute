#include <map>
#include <utils/Prime.h>
#include <vector>

template <int N> struct DataHelper
{
public:
  DataHelper() : mHighestPrimeDiv(N, 1)
  {
    mHighestPrimeDiv[0] = 1;
    mHighestPrimeDiv[1] = 1;
    mGroupByHighestDiv[2].push_back(0);
    mGroupByHighestDiv[2].push_back(1);

    for (int i = 2; i < N; ++i)
    {
      int p               = mPrime.highest_prime_factor(i);
      mHighestPrimeDiv[i] = p;
      mGroupByHighestDiv[p].push_back(i);
    }
  }

  const Prime<N>& prime() const { return mPrime; }
  const std::vector<int>& highestPrimeDiv() const { return mHighestPrimeDiv; }
  const std::map<int, std::vector<int>>& groupByHighestDiv() const { return mGroupByHighestDiv; }

private:
  Prime<N> mPrime;
  std::vector<int> mHighestPrimeDiv;
  std::map<int, std::vector<int>> mGroupByHighestDiv;
};

inline bool has_sum(const std::vector<char>& active, int target)
{
  for (int j = 0; j <= target / 2; j++)
  {
    if (active[j] && active[target - j]) return true;
  }
  return false;
}

inline int bruteforce_answer(const std::vector<char>& active)
{
  for (int i = 0; i < active.size(); i++)
  {
    if (!has_sum(active, i)) return i;
  }
  return -1;
}

template <int N>
int find_answer(const DataHelper<N>& data, int P, int lower_bound, const std::vector<char>& active)
{
  const int n = active.size();

  int first_chunk_size = 0;
  for (int i = 0; i < n; i++)
    if (!active[i])
    {
      first_chunk_size = i;
      break;
    }

  int cand = std::max(2 * first_chunk_size + 1, lower_bound);
  while (cand < n)
  {
    if (active[cand])
      cand += first_chunk_size;
    else if (data.prime().lowest_prime_factor(cand) <= P)
      cand++;
    else if (has_sum(active, cand))
      cand++;
    else
      return cand;
  }

  return -1;
}
