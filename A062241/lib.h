#include <vector>

bool has_sum(const std::vector<char>& active, int target)
{
  for (int j = 0; j <= target / 2; j++)
  {
    if (active[j] && active[target - j]) return true;
  }
  return false;
}

int bruteforce_answer(const std::vector<char>& active)
{
  for (int i = 0; i < active.size(); i++)
  {
    if (!has_sum(active, i)) return i;
  }
  return -1;
}

int find_answer(int lower_bound, const std::vector<char>& active)
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
    else if (!has_sum(active, cand))
      return cand;
    else
      cand++;
  }

  return -1;
}
