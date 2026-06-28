#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <utils/maya/Prime.hpp>

// A CRT residue vector: an integer tracked simultaneously as its residue mod
// each Mod. When the moduli are pairwise coprime this is exactly Z/lcm(Mods)
// (CRT gives a ring isomorphism), so +, -, *, / are that ring's operations --
// in particular / is the modular inverse, not integer division.
//
// We do NOT require the Mods to be prime: many uses (multi-prime hashing, CRT
// reconstruction) never divide, and skipping the primality machinery is faster.
// Division is enabled only when every Mod is prime, so inverses always exist.
//
// The Mods must be pairwise coprime -- otherwise the Z/lcm(Mods) identification
// (and CRT) breaks, e.g. ModInt<7, 7> is not Z/7.
namespace modint_detail {
template <uint32_t... Mods> constexpr bool pairwise_coprime()
{
  constexpr size_t n = sizeof...(Mods);
  constexpr std::array<uint32_t, n> mods{Mods...};
  for (size_t i = 0; i < n; i++)
    for (size_t j = i + 1; j < n; j++)
      if (!maya::is_coprime(mods[i], mods[j])) return false;
  return true;
}
} // namespace modint_detail

template <uint32_t... Mods> class ModInt
{
  static_assert((... && (Mods <= (uint32_t{1} << 31))), "All Mods must be <= 2^31");
  static_assert((... && (0 < Mods)), "All Mods must be positive");
  static_assert(modint_detail::pairwise_coprime<Mods...>(), "All Mods must be pairwise coprime");
  static constexpr size_t K = sizeof...(Mods);

  static constexpr bool cModAreAllPrimes = (... && maya::is_prime(Mods));

public:
  [[nodiscard]] ModInt() { mVals.fill(0); }

  [[nodiscard]] ModInt(uint32_t v) { set_all(v); }

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

  [[nodiscard]] ModInt operator-() const
  {
    ModInt r;
    for_each_mod([&](size_t i, uint32_t Mod)
    {
      uint32_t x = mVals[i];
      r.mVals[i] = (x == 0 ? 0 : Mod - x);
    });
    return r;
  }

  const ModInt& operator+=(const ModInt& other)
  {
    for_each_mod([&](size_t i, uint32_t Mod)
    {
      uint32_t x = mVals[i] + other.mVals[i];
      if (x >= Mod) x -= Mod;
      mVals[i] = x;
    });
    return *this;
  }

  const ModInt& operator-=(const ModInt& other)
  {
    for_each_mod([&](size_t i, uint32_t Mod)
    {
      uint32_t x = mVals[i] + Mod - other.mVals[i];
      if (x >= Mod) x -= Mod;
      mVals[i] = x;
    });
    return *this;
  }

  const ModInt& operator*=(const ModInt& other)
  {
    for_each_mod([&](size_t i, uint32_t Mod)
    {
      uint64_t prod = uint64_t{mVals[i]} * other.mVals[i];
      mVals[i]      = static_cast<uint32_t>(prod % Mod);
    });
    return *this;
  }

  const ModInt& operator/=(const ModInt& other)
    requires(cModAreAllPrimes)
  {
    for_each_mod([&](size_t i, uint32_t Mod)
    {
      uint64_t prod = uint64_t{mVals[i]} * inverse(other.mVals[i], Mod);
      mVals[i]      = static_cast<uint32_t>(prod % Mod);
    });
    return *this;
  }

  [[nodiscard]] ModInt operator+(const ModInt& other) const
  {
    ModInt n = *this;
    n += other;
    return n;
  }

  [[nodiscard]] ModInt operator-(const ModInt& other) const
  {
    ModInt n = *this;
    n -= other;
    return n;
  }

  [[nodiscard]] ModInt operator*(const ModInt& other) const
  {
    ModInt n = *this;
    n *= other;
    return n;
  }

  [[nodiscard]] ModInt operator/(const ModInt& other) const
    requires(cModAreAllPrimes)
  {
    ModInt n = *this;
    n /= other;
    return n;
  }

  [[nodiscard]] bool operator==(const ModInt& other) const
  {
    for (size_t i = 0; i < K; i++)
    {
      if (mVals[i] != other.mVals[i]) return false;
    }
    return true;
  }

private:
  static uint32_t inverse(uint64_t x, uint32_t Mod)
  {
    uint64_t oldR = x % Mod, r = Mod;
    uint64_t oldS = 1, s = 0;
    while (r != 0)
    {
      uint64_t q    = oldR / r;
      uint64_t tmpR = oldR - q * r;
      oldR          = r;
      r             = tmpR;

      uint64_t qs   = q % Mod * s % Mod;
      uint64_t tmpS = (oldS + Mod - qs) % Mod;
      oldS          = s;
      s             = tmpS;
    }
    if (oldR != 1) throw std::invalid_argument("ModInt::inverse: argument not invertible (gcd != 1)");
    return static_cast<uint32_t>(oldS);
  }

  template <typename F> static void for_each_mod(F&& f)
  {
    size_t i = 0;
    (f(i++, Mods), ...);
  }

  void set_all(uint32_t v)
  {
    for_each_mod([&](size_t i, uint32_t Mod) { mVals[i] = v % Mod; });
  }

public:
  std::array<uint32_t, K> mVals;
};
