#pragma once
#include <utils/BigInt.h>

namespace slow_bigint {

TEMPLATE_BIGINT BIGINT::BigInt() : mDigits{} {}

TEMPLATE_BIGINT BIGINT::BigInt(int64_t v)
{
  mIsNeg = v < 0;
  if (mIsNeg) v = -v;

  if (v == 0)
  {
    mDigits.clear();
    return;
  }

  while (v > 0)
  {
    mDigits.push_back(v % Base);
    v /= Base;
  }
}

TEMPLATE_BIGINT BIGINT::BigInt(const std::string& s)
{
  mDigits.clear();
  for (char c : s)
  {
    if (c < '0' || c > '9') continue;
    *this *= 10;
    *this += (c - '0');
  }
  mIsNeg = !s.empty() && s[0] == '-';
}

TEMPLATE_BIGINT BIGINT::BigInt(View v) : mIsNeg{v.is_neg()}
{
  if (v.data() && v.digits() > 0) mDigits.assign(v.data(), v.data() + v.digits());
}

TEMPLATE_BIGINT
template <typename ODigitT, ODigitT OB> BIGINT::BigInt(const BigInt<ODigitT, OB>& other)
{
  mIsNeg = other.is_neg();
  mDigits.clear();

  for (auto it = other.digits().rbegin(); it != other.digits().rend(); ++it)
  {
    *this *= OB;
    *this += *it;
  }

  normalize();
}

template <> template <> DecBigInt::BigInt(const DenseDecBigInt& other)
{
  this->mDigits.clear();
  this->mIsNeg = other.is_neg();
  if (other.digits().empty()) return;

  for (auto block : other.digits())
  {
    for (int j = 0; j < 9; ++j)
    {
      this->mDigits.push_back(static_cast<DecBigInt::Digit>(block % 10));
      block /= 10;
    }
  }
  normalize();
}

template <> template <> DenseDecBigInt::BigInt(const DecBigInt& other)
{
  this->mDigits.clear();
  this->mIsNeg = other.is_neg();
  if (other.digits().empty()) return;

  const auto& sparseDigits = other.digits();

  for (size_t i = 0; i < sparseDigits.size(); i += 9)
  {
    DenseDecBigInt::Digit block = 0;
    for (size_t j = 0; j < 9 && (i + j) < sparseDigits.size(); ++j)
    {
      block *= 10;
      block += static_cast<DenseDecBigInt::Digit>(sparseDigits[i + j]);
    }
    this->mDigits.push_back(block);
  }
}

TEMPLATE_BIGINT
std::ostream& operator<<(std::ostream& os, const BigInt<DigitT, B>& b)
{
  if (b.mIsNeg) os << "-";
  for (auto it = b.mDigits.rbegin(); it != b.mDigits.rend(); ++it)
    if constexpr (B <= 10)
      os << *it;
    else
      os << "(" << *it << ")";
  return os;
}

TEMPLATE_BIGINT void BIGINT::normalize()
{
  while (mDigits.size() >= 1 && mDigits.back() == 0) mDigits.pop_back();
  if (is_zero()) mIsNeg = false;
}

TEMPLATE_BIGINT int BIGINT::abs_cmp(const BigInt& a, const BigInt& b)
{
  if (a.mDigits.size() != b.mDigits.size()) return a.mDigits.size() < b.mDigits.size() ? -1 : 1;

  for (int i = (int)a.mDigits.size() - 1; i >= 0; --i)
    if (a.mDigits[i] != b.mDigits[i]) return a.mDigits[i] < b.mDigits[i] ? -1 : 1;

  return 0;
}

TEMPLATE_BIGINT void BigInt<DigitT, B>::abs_add_with(const BigInt& o)
{
  size_t n = std::max(mDigits.size(), o.mDigits.size());
  mDigits.resize(n, 0);

  DigitT carry = 0;
  for (size_t i = 0; i < n; ++i)
  {
    DigitT sum = carry + mDigits[i] + (i < o.mDigits.size() ? o.mDigits[i] : 0);
    mDigits[i] = sum % Base;
    carry      = sum / Base;
  }
  if (carry) mDigits.push_back(carry);
}

TEMPLATE_BIGINT void BIGINT::abs_sub_with(const BigInt& o)
{
  int64_t carry = 0;
  for (size_t i = 0; i < o.mDigits.size() || carry; ++i)
  {
    int64_t cur = int64_t(mDigits[i]) - (i < o.mDigits.size() ? o.mDigits[i] : 0) - carry;
    carry       = cur < 0;
    if (carry) cur += Base;
    mDigits[i] = cur;
  }
}

TEMPLATE_BIGINT void BIGINT::signed_add_with(const BigInt& o, bool negate_o)
{
  bool oNeg = negate_o ? !o.mIsNeg : o.mIsNeg;

  if (mIsNeg == oNeg)
  {
    abs_add_with(o);
    return;
  }

  int c = abs_cmp(*this, o);
  if (c == 0)
  {
    mDigits.clear();
    mIsNeg = false;
  }
  else if (c > 0)
  {
    abs_sub_with(o);
  }
  else
  {
    BigInt tmp = o;
    tmp.abs_sub_with(*this);
    *this  = std::move(tmp);
    mIsNeg = oNeg;
  }
}

TEMPLATE_BIGINT void BIGINT::div_simple(const BigInt& o)
{
  BigInt rem;
  rem.mDigits.clear();

  for (int i = (int)mDigits.size() - 1; i >= 0; --i)
  {
    rem.mDigits.insert(rem.mDigits.begin(), mDigits[i]);

    Digit lo = 0, hi = Base - 1, x = 0;
    while (lo <= hi)
    {
      Digit mid = lo + (hi - lo) / 2;
      BigInt t  = o * mid;
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
      rem.abs_sub_with(t);
    }
  }
}

TEMPLATE_BIGINT BigInt<DigitT, B> BIGINT::mult_simple(const BigInt& a, const BigInt& b)
{
  BigInt result;
  const auto asize = a.mDigits.size();
  const auto bsize = b.mDigits.size();
  result.mDigits.assign(asize + bsize, 0);

  for (size_t i = 0; i < asize; ++i)
  {
    Digit carry = 0;
    for (size_t j = 0; j < bsize; ++j)
    {
      Digit cur = a.mDigits[i] * b.mDigits[j] + carry;

      result.mDigits[i + j] += cur;
      carry = result.mDigits[i + j] / Base;
      result.mDigits[i + j] %= Base;
    }
    if (carry) result.mDigits[i + bsize] += carry;
  }

  result.mIsNeg = a.mIsNeg ^ b.mIsNeg;
  return result;
}

TEMPLATE_BIGINT
BigInt<DigitT, B> BIGINT::mult_karatsuba(const BigInt& a, const BigInt& b)
{
  if (std::min(a.mDigits.size(), b.mDigits.size()) < KARATSUBA_THRESHOLD_DIGITS) return mult_simple(a, b);

  size_t m = std::max(a.mDigits.size(), b.mDigits.size()) / 2;

  BigInt a0, a1, b0, b1;
  a0.mDigits.assign(a.mDigits.begin(), a.mDigits.begin() + std::min(m, a.mDigits.size()));
  a1.mDigits.assign(a.mDigits.begin() + std::min(m, a.mDigits.size()), a.mDigits.end());
  b0.mDigits.assign(b.mDigits.begin(), b.mDigits.begin() + std::min(m, b.mDigits.size()));
  b1.mDigits.assign(b.mDigits.begin() + std::min(m, b.mDigits.size()), b.mDigits.end());

  BigInt z0 = mult_karatsuba(a0, b0);
  BigInt z2 = mult_karatsuba(a1, b1);

  a0.abs_add_with(a1);
  b0.abs_add_with(b1);

  BigInt z1 = mult_karatsuba(a0, b0);
  z1.abs_sub_with(z0);
  z1.abs_sub_with(z2);

  z2.shift_left(2 * m);
  z1.shift_left(m);

  z0.abs_add_with(z1);
  z0.abs_add_with(z2);
  z0.mIsNeg = a.mIsNeg ^ b.mIsNeg;
  return z0;
}

// https://github.com/indy256/codelibrary/blob/main/cpp/numeric/bigint.cpp + gpt | blackbox tested
TEMPLATE_BIGINT
std::pair<BigInt<DigitT, B>, BigInt<DigitT, B>> BIGINT::divmod(const BigInt& x, const BigInt& y)
{
  using Big = BigInt<DigitT, B>;
  if (y.is_zero()) throw std::runtime_error("Division by zero");
  if (x.is_zero()) return {0, 0};

  Digit norm = Base / (y.digits().back() + 1);
  Big a      = x.abs() * norm;
  Big b      = y.abs() * norm;
  Big q, r;
  q.mDigits.resize(a.digits().size());

  auto bsize = b.digits().size();
  for (int i = a.digits().size() - 1; i >= 0; --i)
  {
    r.shift_left(1);
    r += a.digits()[i];

    Digit s1 = bsize < r.digits().size() ? r.digits()[bsize] : 0;
    Digit s2 = bsize - 1 < r.digits().size() ? r.digits()[bsize - 1] : 0;
    Digit d  = (s1 * B + s2) / b.digits().back();

    r -= b * d;
    while (r < 0)
    {
      r += b;
      --d;
    }

    q.mDigits[i] = d;
  }

  q.mIsNeg = x.is_neg() ^ y.is_neg();
  r.mIsNeg = x.is_neg();

  return {q, r / norm};
}

TEMPLATE_BIGINT BigInt<DigitT, B> BIGINT::abs() const
{
  BigInt r = *this;
  r.mIsNeg = false;
  return r;
}

TEMPLATE_BIGINT BigInt<DigitT, B> BIGINT::operator-() const
{
  if (*this == 0) return 0;
  BigInt r = *this;
  r.mIsNeg = !r.mIsNeg;
  return r;
}

TEMPLATE_BIGINT
std::strong_ordering BIGINT::operator<=>(const BigInt& o) const
{
  if (mIsNeg != o.mIsNeg) return mIsNeg ? std::strong_ordering::less : std::strong_ordering::greater;

  int c = abs_cmp(*this, o);
  if (c == 0) return std::strong_ordering::equal;

  bool less = c < 0;
  return mIsNeg ? (less ? std::strong_ordering::greater : std::strong_ordering::less)
                : (less ? std::strong_ordering::less : std::strong_ordering::greater);
}

TEMPLATE_BIGINT void BIGINT::shift_left(size_t digits)
{
  if (is_zero() || digits == 0) return;
  mDigits.insert(mDigits.begin(), digits, Digit(0));
}

TEMPLATE_BIGINT void BIGINT::shift_right(size_t digits)
{
  if (digits >= mDigits.size())
  {
    mDigits.clear();
    mIsNeg = false;
    return;
  }
  mDigits.erase(mDigits.begin(), mDigits.begin() + digits);
}

} // namespace slow_bigint
