#pragma once

#include <array>
#include <compare>
#include <iostream>
#include <limits>

template <int... MODs> class ModInt
{
  static_assert((... && (MODs < std::numeric_limits<int>::max())),
                "All MODs must fit in int");
  static_assert((... && (0 < MODs)), "All MODs must be positive");
  static constexpr size_t K = sizeof...(MODs);

public:
  ModInt() { mVals.fill(0); }

  ModInt(int v) { set_all(v); }

  friend std::ostream& operator<<(std::ostream& out, const ModInt& m)
  {
    out << '{';
    for (size_t i = 0; i < K; i++)
    {
      out << m.mVals[i];
      if (i + 1 < K) out << ", ";
    }
    return out << '}';
  }

  ModInt operator-() const
  {
    ModInt r;
    for_each_mod(
        [&](size_t i, int MOD)
        {
          int x      = mVals[i];
          r.mVals[i] = (x == 0 ? 0 : MOD - x);
        });
    return r;
  }

  const ModInt& operator+=(const ModInt& other)
  {
    for_each_mod(
        [&](size_t i, int MOD)
        {
          int x = mVals[i] + other.mVals[i];
          if (x >= MOD) x -= MOD;
          mVals[i] = x;
        });
    return *this;
  }

  const ModInt& operator-=(const ModInt& other)
  {
    for_each_mod(
        [&](size_t i, int MOD)
        {
          int x = mVals[i] - other.mVals[i];
          if (x < 0) x += MOD;
          mVals[i] = x;
        });
    return *this;
  }

  const ModInt& operator*=(const ModInt& other)
  {
    for_each_mod(
        [&](size_t i, int MOD)
        {
          long long prod = 1LL * mVals[i] * other.mVals[i];
          mVals[i]       = static_cast<int>(prod % MOD);
        });
    return *this;
  }

  ModInt operator+(const ModInt& other)
  {
    ModInt n = *this;
    n += other;
    return n;
  }

  ModInt operator-(const ModInt& other)
  {
    ModInt n = *this;
    n -= other;
    return n;
  }

  ModInt operator*(const ModInt& other)
  {
    ModInt n = *this;
    n *= other;
    return n;
  }

  // returns false => not equal
  // returns true => probably equal
  bool operator==(const ModInt& other) const
  {
    for (size_t i = 0; i < K; i++)
    {
      if (mVals[i] != other.mVals[i]) return false;
    }
    return true;
  }

private:
  template <typename F> static void for_each_mod(F&& f)
  {
    static constexpr int mods[] = {MODs...};
    for (size_t i = 0; i < K; i++)
    {
      f(i, mods[i]);
    }
  }

  static int normalize(long long x, int MOD)
  {
    x %= MOD;
    if (x < 0) x += MOD;
    return static_cast<int>(x);
  }

  void set_all(int v)
  {
    for_each_mod([&](size_t i, int MOD) { mVals[i] = normalize(v, MOD); });
  }

public:
  std::array<int, K> mVals;
};
