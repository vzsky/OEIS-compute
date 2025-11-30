#include "BigInt.h"
#include <cctype>

/**************************************
 * CONSTRUCTIONS
 *************************************/

BigInt::BigInt() : mIsNegative(false), mDigits{0}
{
}

BigInt::BigInt(int x)
{
  mIsNegative = x < 0;
  if (mIsNegative)
    x = -x;

  if (x == 0)
  {
    mDigits = {0};
    return;
  }
  while (x > 0)
  {
    mDigits.push_back(char(x % 10));
    x /= 10;
  }
}

BigInt::BigInt(const std::string& s)
{
  mDigits.clear();
  mIsNegative = s[0] == '-';

  size_t i = mIsNegative ? 1 : 0;
  for (; i < s.size(); i++)
  {
    if (std::isdigit(s[i]))
      mDigits.push_back(char(s[i] - '0'));
  }

  if (mDigits.empty())
    mDigits = {0};
  std::reverse(mDigits.begin(), mDigits.end());
  remove_leading_zeros();
}

/**************************************
 * OPERATIONS
 *************************************/

void BigInt::remove_leading_zeros()
{
  while (mDigits.size() > 1 && mDigits.back() == 0)
    mDigits.pop_back();

  if (mDigits.size() == 1 && mDigits[0] == 0)
    mIsNegative = false;
}

void BigInt::abs_add(const BigInt& src)
{
  int carry = 0;
  size_t n = std::max(mDigits.size(), src.mDigits.size());

  mDigits.resize(n, 0);

  for (size_t i = 0; i < n; ++i)
  {
    int d = mDigits[i] + (i < src.mDigits.size() ? src.mDigits[i] : 0) + carry;
    mDigits[i] = d % 10;
    carry = d / 10;
  }
  if (carry)
    mDigits.push_back(carry);
}

void BigInt::abs_sub(const BigInt& src)
{
  int borrow = 0;
  for (size_t i = 0; i < mDigits.size(); ++i)
  {
    int d = mDigits[i] - borrow - (i < src.mDigits.size() ? src.mDigits[i] : 0);
    if (d < 0)
    {
      d += 10;
      borrow = 1;
    }
    else
    {
      borrow = 0;
    }
    mDigits[i] = d;
  }
  remove_leading_zeros();
}

BigInt BigInt::abs() const
{
  BigInt result = *this;
  result.mIsNegative = false;
  return result;
}

BigInt BigInt::operator-() const
{
  BigInt result = *this;
  if (result != BigInt(0))
    result.mIsNegative = !result.mIsNegative;
  return result;
}

std::ostream& operator<<(std::ostream& os, const BigInt& x)
{
  if (x.mDigits.empty())
    return os << 0;
  if (x.mIsNegative)
    os << '-';

  for (auto it = x.mDigits.rbegin(); it != x.mDigits.rend(); ++it)
    os << char('0' + *it);

  return os;
}

const BigInt& BigInt::operator+=(const BigInt& other)
{
  if (mIsNegative == other.mIsNegative)
  {
    abs_add(other);
  }
  else
  {
    if (abs() < other.abs())
    {
      BigInt tmp = other;
      tmp.abs_sub(*this);
      tmp.mIsNegative = other.mIsNegative;
      *this = tmp;
    }
    else
    {
      abs_sub(other);
    }
  }
  remove_leading_zeros();
  return *this;
}

const BigInt& BigInt::operator-=(const BigInt& other)
{
  *this += (-other);
  return *this;
}

const BigInt& BigInt::operator*=(const BigInt& other)
{
  if (*this == BigInt(0) || other == BigInt(0))
  {
    *this = BigInt(0);
    return *this;
  }

  std::vector<char> result(mDigits.size() + other.mDigits.size(), 0);

  for (size_t i = 0; i < mDigits.size(); ++i)
  {
    int carry = 0;
    for (size_t j = 0; j < other.mDigits.size() || carry; ++j)
    {
      int sum =
          result[i + j] + mDigits[i] * (j < other.mDigits.size() ? other.mDigits[j] : 0) + carry;
      result[i + j] = sum % 10;
      carry = sum / 10;
    }
  }

  mDigits = result;
  remove_leading_zeros();
  mIsNegative = mIsNegative != other.mIsNegative;
  return *this;
}

const BigInt& BigInt::operator%=(const BigInt& other)
{
  while (*this >= 0)
    *this -= other;
  *this += other;
  return *this;
}

BigInt BigInt::operator+(const BigInt& o)
{
  BigInt n = *this;
  n += o;
  return n;
}

BigInt BigInt::operator-(const BigInt& o)
{
  BigInt n = *this;
  n -= o;
  return n;
}

BigInt BigInt::operator*(const BigInt& o)
{
  BigInt n = *this;
  n *= o;
  return n;
}

BigInt BigInt::operator%(const BigInt& o)
{
  BigInt n = *this;
  n %= o;
  return n;
}

std::strong_ordering BigInt::operator<=>(const BigInt& other) const
{
  if (mIsNegative != other.mIsNegative)
    return mIsNegative ? std::strong_ordering::less : std::strong_ordering::greater;

  if (mDigits.size() != other.mDigits.size())
  {
    if (mIsNegative)
      return mDigits.size() > other.mDigits.size() ? std::strong_ordering::less
                                                   : std::strong_ordering::greater;
    else
      return mDigits.size() < other.mDigits.size() ? std::strong_ordering::less
                                                   : std::strong_ordering::greater;
  }

  for (size_t i = mDigits.size(); i > 0; --i)
  {
    if (mDigits[i - 1] != other.mDigits[i - 1])
    {
      if (mIsNegative)
        return mDigits[i - 1] > other.mDigits[i - 1] ? std::strong_ordering::less
                                                     : std::strong_ordering::greater;
      else
        return mDigits[i - 1] < other.mDigits[i - 1] ? std::strong_ordering::less
                                                     : std::strong_ordering::greater;
    }
  }

  return std::strong_ordering::equal;
}
