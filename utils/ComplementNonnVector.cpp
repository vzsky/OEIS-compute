#include <algorithm>
#include <utils/ComplementNonnVector.h>

using namespace complementNonnVector;

size_t Vector::count_complement_lt(value_type num) const
{
  return std::lower_bound(mComplement.begin(), mComplement.end(), num) - mComplement.begin();
}

size_t Vector::count_sequence_lt(value_type num) const { return num - count_complement_lt(num); }

value_type Vector::operator[](size_t idx) const
{
  value_type lo = 0;
  value_type hi = idx + mComplement.size() + 1;

  while (lo < hi)
  {
    value_type mid = lo + (hi - lo) / 2;
    if (count_sequence_lt(mid) <= idx)
      lo = mid + 1;
    else
      hi = mid;
  }

  return lo - 1;
}

void Vector::add_skip(value_type n)
{
  assert(mComplement.empty() || n > mComplement.back());
  mComplement.push_back(n);
}

/***********************
 * Iterators
 ***********************/

Vector::iterator::iterator(const Vector* seq, size_t index) : mSeq(seq), mIndex(index)
{
  if (mSeq && mIndex < mSeq->count_sequence_lt(UINT64_MAX))
  {
    mValue           = (*mSeq)[mIndex];
    mComplementIndex = std::upper_bound(mSeq->mComplement.begin(), mSeq->mComplement.end(), mValue) -
                       mSeq->mComplement.begin();
  }
}

Vector::iterator::reference Vector::iterator::operator*() const { return mValue; }

Vector::iterator& Vector::iterator::operator++()
{
  ++mIndex;
  ++mValue;
  while (mComplementIndex < mSeq->mComplement.size() && mValue == mSeq->mComplement[mComplementIndex])
  {
    ++mValue;
    ++mComplementIndex;
  }

  return *this;
}

Vector::iterator& Vector::iterator::operator--()
{
  --mIndex;
  --mValue;
  while (mComplementIndex > 0 && mValue == mSeq->mComplement[mComplementIndex - 1])
  {
    --mValue;
    --mComplementIndex;
  }

  return *this;
}

bool Vector::iterator::operator==(const iterator& o) const { return mSeq == o.mSeq && mIndex == o.mIndex; }

bool Vector::iterator::operator!=(const iterator& o) const { return !(*this == o); }
