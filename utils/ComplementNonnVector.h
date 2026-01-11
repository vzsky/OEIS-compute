#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace complementNonnVector
{
using value_type = uint64_t;

class Vector
{
public:
  class iterator
  {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = complementNonnVector::value_type;
    using reference         = value_type;
    using pointer           = value_type*;

    iterator() = default;

    iterator(const Vector* seq, size_t index);

    reference operator*() const;
    iterator& operator++();
    iterator& operator--();
    bool operator==(const iterator& o) const;
    bool operator!=(const iterator& o) const;

    int idx() const { return mIndex; }

  private:
    const Vector* mSeq      = nullptr;
    int mIndex              = 0;
    value_type mValue       = 0;
    size_t mComplementIndex = 0;
  };

private:
  size_t count_complement_lt(value_type num) const;
  size_t count_sequence_lt(value_type num) const;

public:
  value_type operator[](size_t idx) const;
  inline size_t lower_bound(value_type value) const { return count_sequence_lt(value); }
  inline size_t upper_bound(value_type value) const { return count_sequence_lt(value + 1); }
  void add_skip(value_type n);

  iterator it_at(size_t idx) const { return iterator{this, idx}; }

private:
  std::vector<value_type> mComplement{};
};

} // namespace complementNonnVector
