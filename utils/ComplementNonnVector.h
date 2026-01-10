#pragma once

#include <cassert>
#include <vector>

// this is an infinite sorted vector
class ComplementNonnVector
{
private:
  size_t count_complement_lt(uint64_t num) const
  {
    return std::lower_bound(mComplement.begin(), mComplement.end(), num) - mComplement.begin();
  }

  size_t count_sequence_lt(uint64_t num) const { return num - count_complement_lt(num); }

public:
  uint64_t operator[](size_t idx) const
  {
    uint64_t lo = 0;
    uint64_t hi = idx + mComplement.size() + 1;

    while (lo < hi)
    {
      uint64_t mid = lo + (hi - lo) / 2;
      if (count_sequence_lt(mid) <= idx)
        lo = mid + 1;
      else
        hi = mid;
    }

    return lo - 1;
  }

  size_t lower_bound(uint64_t value) { return count_sequence_lt(value); }

  size_t upper_bound(uint64_t value) { return count_sequence_lt(value + 1); }

  void add_skip(uint64_t n)
  {
    assert(mComplement.empty() || n > mComplement.back());
    mComplement.push_back(n);
  }

private:
  std::vector<uint64_t> mComplement{};
};
