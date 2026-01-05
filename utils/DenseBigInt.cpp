#include <algorithm>
#include <cassert>
#include <utils/DenseBigInt.h>
#include <vector>

//
// Constructors
//
DenseBackend::DenseBackend()
{
  mDigits = {0};
  mIsNeg = false;
}

DenseBackend::DenseBackend(int v)
{
  mIsNeg = v < 0;
  if (mIsNeg)
    v = -v;

  if (v == 0)
  {
    mDigits = {0};
    return;
  }

  while (v > 0)
  {
    mDigits.push_back(static_cast<Digit>(v & 0xff));
    v >>= 8;
  }
}

//
// normalize
//
void DenseBackend::normalize()
{
  while (mDigits.size() > 1 && mDigits.back() == 0)
    mDigits.pop_back();

  if (mDigits.size() == 1 && mDigits[0] == 0)
    mIsNeg = false;
}

//
// abs compare
//
int DenseBackend::abs_cmp(const DenseBackend& a, const DenseBackend& b)
{
  if (a.mDigits.size() != b.mDigits.size())
    return a.mDigits.size() < b.mDigits.size() ? -1 : 1;

  for (int i = (int) a.mDigits.size() - 1; i >= 0; --i)
  {
    if (a.mDigits[i] != b.mDigits[i])
      return a.mDigits[i] < b.mDigits[i] ? -1 : 1;
  }
  return 0;
}

//
// |this| += |o|
//
void DenseBackend::abs_add(const DenseBackend& o)
{
  size_t n = std::max(mDigits.size(), o.mDigits.size());
  mDigits.resize(n, 0);

  uint16_t carry = 0;
  for (size_t i = 0; i < n || carry; ++i)
  {
    if (i == mDigits.size())
      mDigits.push_back(0);

    uint16_t sum = carry + mDigits[i] + (i < o.mDigits.size() ? o.mDigits[i] : 0);
    mDigits[i] = static_cast<Digit>(sum);
    carry = sum >> 8;
  }
}

//
// |this| -= |o|  (assumes |this| >= |o|)
//
void DenseBackend::abs_sub(const DenseBackend& o)
{
  int carry = 0;
  for (size_t i = 0; i < o.mDigits.size() || carry; ++i)
  {
    int cur = int(mDigits[i]) - (i < o.mDigits.size() ? o.mDigits[i] : 0) - carry;
    carry = cur < 0;
    if (carry)
      cur += 256;
    mDigits[i] = static_cast<Digit>(cur);
  }
  normalize();
}

//
// signed add
//
void DenseBackend::add(const DenseBackend& o)
{
  if (mIsNeg == o.mIsNeg)
  {
    abs_add(o);
  }
  else
  {
    int c = abs_cmp(*this, o);
    if (c == 0)
    {
      mDigits = {0};
      mIsNeg = false;
    }
    else if (c > 0)
    {
      abs_sub(o);
    }
    else
    {
      DenseBackend tmp = o;
      tmp.abs_sub(*this);
      *this = tmp;
    }
  }
}

//
// signed sub
//
void DenseBackend::sub(const DenseBackend& o)
{
  DenseBackend t = o;
  t.mIsNeg = !t.mIsNeg;
  add(t);
}

//
// schoolbook mul
//
void DenseBackend::mul(const DenseBackend& o)
{
  if ((mDigits.size() == 1 && mDigits[0] == 0) || (o.mDigits.size() == 1 && o.mDigits[0] == 0))
  {
    mDigits = {0};
    mIsNeg = false;
    return;
  }

  std::vector<uint16_t> tmp(mDigits.size() + o.mDigits.size(), 0);

  for (size_t i = 0; i < mDigits.size(); ++i)
  {
    uint16_t carry = 0;
    for (size_t j = 0; j < o.mDigits.size() || carry; ++j)
    {
      uint32_t cur =
          tmp[i + j] + uint32_t(mDigits[i]) * (j < o.mDigits.size() ? o.mDigits[j] : 0) + carry;

      tmp[i + j] = cur & 0xff;
      carry = cur >> 8;
    }
  }

  mDigits.resize(tmp.size());
  for (size_t i = 0; i < tmp.size(); ++i)
    mDigits[i] = static_cast<Digit>(tmp[i]);

  mIsNeg ^= o.mIsNeg;
  normalize();
}

//
// mod: naive repeated subtraction (replace with real div later)
//
void DenseBackend::mod(const DenseBackend& o)
{
  if (o.mDigits.size() == 1 && o.mDigits[0] == 0)
    throw std::runtime_error("mod by zero");

  if (o.mIsNeg)
    throw std::runtime_error("mod by negative");

  while (mIsNeg)
    add(o);
  while (abs_cmp(*this, o) >= 0)
    abs_sub(o);
}

//
// shifts (base-256)
//
void DenseBackend::shl(int k)
{
  if (mDigits.size() == 1 && mDigits[0] == 0)
    return;
  mDigits.insert(mDigits.begin(), k, 0);
}

void DenseBackend::shr(int k)
{
  if (k >= (int) mDigits.size())
  {
    mDigits = {0};
    mIsNeg = false;
    return;
  }
  mDigits.erase(mDigits.begin(), mDigits.begin() + k);
  normalize();
}
