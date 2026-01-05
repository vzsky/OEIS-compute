#include <algorithm>
#include <cassert>
#include <cctype>
#include <stdexcept>
#include <utils/DecBigInt.h>
#include <vector>

//
// Constructors
//
DecBackend::DecBackend()
{
  mDigits = {0};
  mIsNeg = false;
}

DecBackend::DecBackend(int v)
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
    mDigits.push_back(v % 10);
    v /= 10;
  }
}

DecBackend::DecBackend(const std::string& s)
{
  if (s.empty())
  {
    mDigits = {0};
    mIsNeg = false;
    return;
  }

  int i = 0;
  mIsNeg = false;
  if (s[0] == '-')
  {
    mIsNeg = true;
    i = 1;
  }

  mDigits.clear();
  for (; i < (int) s.size(); i++)
  {
    char c = s[i];
    if (c < '0' || c > '9')
      continue;
    mDigits.push_back(c - '0');
  }

  std::reverse(mDigits.begin(), mDigits.end());
  normalize();
}

//
// Remove leading zeros
//
void DecBackend::normalize()
{
  if (mDigits.size() == 0)
    mDigits = {0};

  while (mDigits.size() > 1 && mDigits.back() == 0)
    mDigits.pop_back();

  if (mDigits.size() == 1 && mDigits[0] == 0)
    mIsNeg = false;
}

//
// Compare absolute values
//
int DecBackend::abs_cmp(const DecBackend& a, const DecBackend& b)
{
  if (a.mDigits.size() != b.mDigits.size())
    return (a.mDigits.size() < b.mDigits.size()) ? -1 : 1;
  for (int i = (int) a.mDigits.size() - 1; i >= 0; i--)
  {
    if (a.mDigits[i] != b.mDigits[i])
      return (a.mDigits[i] < b.mDigits[i]) ? -1 : 1;
  }
  return 0;
}

//
// abs_add: |this| += |other|
//
void DecBackend::abs_add(const DecBackend& o)
{
  size_t n = std::max(mDigits.size(), o.mDigits.size());
  mDigits.resize(n, 0);

  int carry = 0;
  for (size_t i = 0; i < n || carry; i++)
  {
    if (i == mDigits.size())
      mDigits.push_back(0);
    int sum = mDigits[i] + (i < o.mDigits.size() ? o.mDigits[i] : 0) + carry;
    mDigits[i] = sum % 10;
    carry = sum / 10;
  }
}

//
// abs_sub: |this| -= |other|, assuming |this| >= |other|
//
void DecBackend::abs_sub(const DecBackend& o)
{
  int carry = 0;
  for (size_t i = 0; i < o.mDigits.size() || carry; i++)
  {
    int cur = mDigits[i] - (i < o.mDigits.size() ? o.mDigits[i] : 0) - carry;
    carry = cur < 0;
    if (carry)
      cur += 10;
    mDigits[i] = cur;
  }
  normalize();
}

//
// add: signed addition
//
void DecBackend::add(const DecBackend& o)
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
      DecBackend tmp = o;
      tmp.abs_sub(*this);
      *this = tmp;
    }
  }
}

//
// sub: signed subtraction
//
void DecBackend::sub(const DecBackend& o)
{
  DecBackend t = o;
  t.mIsNeg = !t.mIsNeg;
  add(t);
}

//
// mul: schoolbook multiplication
//
void DecBackend::mul(const DecBackend& o)
{
  if ((mDigits.size() == 1 && mDigits[0] == 0) || (o.mDigits.size() == 1 && o.mDigits[0] == 0))
  {
    mDigits = {0};
    mIsNeg = false;
    return;
  }

  std::vector<uint8_t> res(mDigits.size() + o.mDigits.size(), 0);

  for (size_t i = 0; i < mDigits.size(); i++)
  {
    int carry = 0;
    for (size_t j = 0; j < o.mDigits.size() || carry; j++)
    {
      long long cur =
          res[i + j] + mDigits[i] * 1LL * (j < o.mDigits.size() ? o.mDigits[j] : 0) + carry;
      res[i + j] = int(cur % 10);
      carry = int(cur / 10);
    }
  }

  mDigits = res;
  mIsNeg = (mIsNeg != o.mIsNeg);
  normalize();
}

//
// mod: naive repeated subtraction (replace with real div later)
//
void DecBackend::mod(const DecBackend& o)
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
// shl: decimal shift left = prepend zeros (multiply by 10^k)
//
void DecBackend::shl(int k)
{
  if (mDigits.size() == 1 && mDigits[0] == 0)
    return;
  mDigits.insert(mDigits.begin(), k, 0);
}

//
// shr: decimal shift right = remove k mDigits (divide by 10^k)
//
void DecBackend::shr(int k)
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
