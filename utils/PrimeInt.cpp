#include <stdexcept>
#include <utils/PrimeInt.h>

const Prime<PrimeInt::SmallN> PrimeInt::factorizer{};

std::ostream& operator<<(std::ostream& out, const PrimeInt& m)
{
  bool first = true;
  for (const auto& [p, e] : m.mFactors)
  {
    if (e == 0) continue;
    if (!first) out << " * ";
    out << p;
    if (e > 1) out << "^" << e;
    first = false;
  }
  if (first) out << 1;
  return out;
}

const PrimeInt& PrimeInt::operator*=(const PrimeInt& other)
{
  mTmp.clear();

  size_t i = 0, j = 0;
  while (i < mFactors.size() && j < other.mFactors.size())
  {
    if (mFactors[i].first < other.mFactors[j].first)
      mTmp.push_back(mFactors[i++]);
    else if (mFactors[i].first > other.mFactors[j].first)
      mTmp.push_back(other.mFactors[j++]);
    else if (mFactors[i].first == other.mFactors[j].first)
    {
      mTmp.emplace_back(mFactors[i].first, mFactors[i].second + other.mFactors[j].second);
      i++;
      j++;
    }
  }

  while (i < mFactors.size()) mTmp.push_back(mFactors[i++]);
  while (j < other.mFactors.size()) mTmp.push_back(other.mFactors[j++]);

  mFactors.swap(mTmp);
  return *this;
}

PrimeInt PrimeInt::operator*(const PrimeInt& other) const
{
  PrimeInt n = *this;
  n *= other;
  return n;
}

const PrimeInt& PrimeInt::operator/=(const PrimeInt& other)
{
  mTmp.clear();
  size_t i = 0, j = 0;
  while (i < mFactors.size())
  {
    if (j < other.mFactors.size() && mFactors[i].first == other.mFactors[j].first)
    {
      int e = mFactors[i].second - other.mFactors[j].second;
      assert(e >= 0 && "dividing numbers that are not divisible");
      if (e > 0) mTmp.emplace_back(mFactors[i].first, e);
      i++;
      j++;
    }
    else
    {
      mTmp.push_back(mFactors[i++]);
    }
  }

  if (j != other.mFactors.size()) throw std::invalid_argument("dividing numbers that are not divisible");

  mFactors.swap(mTmp);
  return *this;
}

PrimeInt PrimeInt::operator/(const PrimeInt& other) const
{
  PrimeInt n = *this;
  n /= other;
  return n;
}

bool PrimeInt::is_divisible_by(const PrimeInt& other) const
{
  size_t i = 0, j = 0;
  while (j < other.mFactors.size())
  {
    if (i == mFactors.size()) return false;

    if (mFactors[i].first > other.mFactors[j].first) return false;

    if (mFactors[i].first < other.mFactors[j].first)
      i++;

    else
    { // same prime
      if (mFactors[i].second < other.mFactors[j].second) return false;
      i++;
      j++;
    }
  }
  return true;
}

bool PrimeInt::operator==(const PrimeInt& other) const { return mFactors == other.mFactors; }
