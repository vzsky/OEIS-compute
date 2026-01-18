#pragma once
#include <utils/BigInt.h>

template <typename DigitT, DigitT B> BigInt<DigitT, B>::BigInt() { mDigits = {0}; }

template <typename DigitT, DigitT B> BigInt<DigitT, B>::BigInt(int64_t v)
{
  mIsNeg = v < 0;
  if (mIsNeg) v = -v;

  if (v == 0)
  {
    mDigits = {0};
    return;
  }

  while (v > 0)
  {
    mDigits.push_back(v % Base);
    v /= Base;
  }
}

template <typename DigitT, DigitT B> BigInt<DigitT, B>::BigInt(const std::string& s)
{
  mDigits = {0};
  for (char c : s)
  {
    if (c < '0' || c > '9') continue;
    *this *= 10;
    *this += (c - '0');
  }
  mIsNeg = !s.empty() && s[0] == '-';
}

template <typename DigitT, DigitT B>
template <typename ODigitT, ODigitT OB>
BigInt<DigitT, B>::BigInt(const BigInt<ODigitT, OB>& other)
{
  mIsNeg  = other.is_neg();
  mDigits = {0};

  for (auto it = other.digits().rbegin(); it != other.digits().rend(); ++it)
  {
    *this *= OB;
    *this += *it;
  }

  normalize();
}

template <typename DigitT, DigitT B> void BigInt<DigitT, B>::normalize()
{
  while (mDigits.size() > 1 && mDigits.back() == 0) mDigits.pop_back();
  if (is_zero()) mIsNeg = false;
}

template <typename DigitT, DigitT B> int BigInt<DigitT, B>::abs_cmp(const BigInt& a, const BigInt& b)
{
  if (a.mDigits.size() != b.mDigits.size()) return a.mDigits.size() < b.mDigits.size() ? -1 : 1;

  for (int i = (int)a.mDigits.size() - 1; i >= 0; --i)
    if (a.mDigits[i] != b.mDigits[i]) return a.mDigits[i] < b.mDigits[i] ? -1 : 1;

  return 0;
}

template <typename DigitT, DigitT B> void BigInt<DigitT, B>::abs_add(const BigInt& o)
{
  size_t n = std::max(mDigits.size(), o.mDigits.size());
  mDigits.resize(n, 0);

  uint64_t carry = 0;
  for (size_t i = 0; i < n || carry; ++i)
  {
    if (i == mDigits.size()) mDigits.push_back(0);
    uint64_t sum = carry + mDigits[i] + (i < o.mDigits.size() ? o.mDigits[i] : 0);
    mDigits[i]   = sum % Base;
    carry        = sum / Base;
  }
}

template <typename DigitT, DigitT B> void BigInt<DigitT, B>::abs_sub(const BigInt& o)
{
  int64_t carry = 0;
  for (size_t i = 0; i < o.mDigits.size() || carry; ++i)
  {
    int64_t cur = int64_t(mDigits[i]) - (i < o.mDigits.size() ? o.mDigits[i] : 0) - carry;
    carry       = cur < 0;
    if (carry) cur += Base;
    mDigits[i] = cur;
  }
  normalize();
}

template <typename DigitT, DigitT B> void BigInt<DigitT, B>::signed_add(const BigInt& o, bool negate_o)
{
  bool oNeg = negate_o ? !o.mIsNeg : o.mIsNeg;

  if (mIsNeg == oNeg)
  {
    abs_add(o);
    return;
  }

  int c = abs_cmp(*this, o);
  if (c == 0)
  {
    mDigits = {0};
    mIsNeg  = false;
  }
  else if (c > 0)
  {
    abs_sub(o);
  }
  else
  {
    BigInt tmp = o;
    tmp.abs_sub(*this);
    *this  = tmp;
    mIsNeg = oNeg;
  }
}

template <typename DigitT, DigitT B> BigInt<DigitT, B> BigInt<DigitT, B>::abs_mod_div(const BigInt& o)
{
  BigInt rem;
  rem.mDigits = {0};

  for (int i = (int)mDigits.size() - 1; i >= 0; --i)
  {
    rem.mDigits.insert(rem.mDigits.begin(), mDigits[i]);
    rem.normalize();

    Digit lo = 0, hi = Base - 1, x = 0;
    while (lo <= hi)
    {
      Digit mid = lo + (hi - lo) / 2;
      BigInt t  = o;
      t *= BigInt(mid);
      if (abs_cmp(rem, t) >= 0)
      {
        x  = mid;
        lo = mid + 1;
      }
      else
      {
        if (mid == 0) break;
        hi = mid - 1;
      }
    }

    mDigits[i] = x;
    if (x != 0)
    {
      BigInt t = o;
      t *= BigInt(x);
      rem.abs_sub(t);
    }
  }

  normalize();
  rem.normalize();
  return rem;
}

template <typename DigitT, DigitT B> BigInt<DigitT, B> BigInt<DigitT, B>::mult_simple(const BigInt& o) const
{
  BigInt result;
  result.mDigits.resize(mDigits.size() + o.mDigits.size());

  for (size_t i = 0; i < mDigits.size(); ++i)
    for (size_t j = 0; j < o.mDigits.size(); ++j)
      result.mDigits[i + j] += uint64_t(mDigits[i]) * o.mDigits[j];

  uint64_t carry = 0;
  for (size_t i = 0; i < result.mDigits.size(); ++i)
  {
    uint64_t tmp      = result.mDigits[i] + carry;
    result.mDigits[i] = tmp % Base;
    carry             = tmp / Base;
  }

  result.mIsNeg = mIsNeg ^ o.mIsNeg;
  result.normalize();
  return result;
}

// https://github.com/indy256/codelibrary/blob/main/cpp/numeric/bigint.cpp + gpt | blackbox tested
template <typename DigitT, DigitT B>
std::pair<BigInt<DigitT, B>, BigInt<DigitT, B>> BigInt<DigitT, B>::divmod(const BigInt& a1, const BigInt& b1)
{
  using Big = BigInt<DigitT, B>;
  if (b1.is_zero()) throw std::runtime_error("Division by zero");
  if (a1.is_zero()) return {0, 0};

  int norm = Base / (b1.digits().back() + 1);
  Big a    = a1.abs() * norm;
  Big b    = b1.abs() * norm;
  Big q, r;
  q.mDigits.resize(a.digits().size());

  for (int i = a.digits().size() - 1; i >= 0; --i)
  {
    r *= Base;
    r += a.digits()[i];

    DigitT s1 = b.digits().size() < r.digits().size() ? r.digits()[b.digits().size()] : 0;
    DigitT s2 = b.digits().size() - 1 < r.digits().size() ? r.digits()[b.digits().size() - 1] : 0;
    DigitT d  = (uint64_t(s1) * B + s2) / b.digits().back();

    r -= b * d;
    while (r < 0)
    {
      r += b;
      --d;
    }

    q.mDigits[i] = d;
  }

  q.mIsNeg = a1.is_neg() ^ b1.is_neg();
  r.mIsNeg = a1.is_neg();
  q.normalize();
  r.normalize();

  return {q, r / norm};
}

template <typename DigitT, DigitT B> BigInt<DigitT, B> BigInt<DigitT, B>::abs() const
{
  BigInt r = *this;
  r.mIsNeg = false;
  return r;
}

template <typename DigitT, DigitT B> BigInt<DigitT, B> BigInt<DigitT, B>::operator-() const
{
  if (*this == 0) return *this;
  BigInt r = *this;
  r.mIsNeg = !r.mIsNeg;
  return r;
}

template <typename DigitT, DigitT B>
std::strong_ordering BigInt<DigitT, B>::operator<=>(const BigInt& o) const
{
  if (mIsNeg != o.mIsNeg) return mIsNeg ? std::strong_ordering::less : std::strong_ordering::greater;

  int c = abs_cmp(*this, o);
  if (c == 0) return std::strong_ordering::equal;

  bool less = c < 0;
  return mIsNeg ? (less ? std::strong_ordering::greater : std::strong_ordering::less)
                : (less ? std::strong_ordering::less : std::strong_ordering::greater);
}

template <typename DigitT, DigitT B> bool BigInt<DigitT, B>::is_zero() const
{
  return mDigits.size() == 1 && mDigits[0] == 0;
}

namespace math
{
template <typename DigitT, DigitT B> BigInt<DigitT, B> pow(BigInt<DigitT, B> a, uint64_t exp)
{
  BigInt<DigitT, B> r(1);
  while (exp)
  {
    if (exp & 1) r *= a;
    a *= a;
    exp >>= 1;
  }
  return r;
}
} // namespace math
