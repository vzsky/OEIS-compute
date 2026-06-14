#pragma once

#include <algorithm>
#include <cassert>
#include <complex>
#include <cstdint>
#include <numbers>
#include <vector>

#include <utils/Logging.hpp>

namespace fft {

using cd = std::complex<double>;

template <std::integral T> [[nodiscard]] std::vector<T> round(const std::vector<cd>& v)
{
  std::vector<T> result;
  for (size_t i = 0; i < v.size(); i++) result.push_back(static_cast<T>(v[i].real() + 0.5));
  return result;
}

template <std::integral T> [[nodiscard]] std::vector<T> round(const std::vector<double>& v)
{
  std::vector<T> result;
  for (size_t i = 0; i < v.size(); i++) result.push_back(static_cast<T>(v[i] + 0.5));
  return result;
}

template <typename U, typename T> [[nodiscard]] std::vector<U> fmap_cast(const std::vector<T>& v)
{
  if constexpr (std::is_same_v<U, int> && std::is_same_v<T, double>)
  {
    Log(LL::Infra, "cast'ing double to int. Are you sure you don't want round?");
  }
  std::vector<U> result;
  for (auto x : v) result.push_back(static_cast<U>(x));
  return result;
}

[[nodiscard]] inline std::vector<uint64_t> reverse_bit(size_t n)
{
  std::vector<uint64_t> rev;
  size_t k = __builtin_ctz(n);
  rev.assign(n, 0);
  for (size_t i = 0; i < n; i++) rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (k - 1));
  return rev;
}

[[nodiscard]] inline std::vector<cd> unity_roots(size_t n)
{
  std::vector<cd> roots{{0, 0}, {1, 0}};
  if (roots.size() < n)
  {
    int k = __builtin_ctz(roots.size());
    roots.resize(n);
    while ((1 << k) < n)
    {
      double angle = 2 * std::numbers::pi / (1 << (k + 1));
      for (int i = 1 << (k - 1); i < (1 << k); i++)
      {
        roots[2 * i]     = roots[i];
        double ang       = angle * (2 * i + 1 - (1 << k));
        roots[2 * i + 1] = cd(cos(ang), sin(ang));
      }
      k++;
    }
  }
  return roots;
}

[[nodiscard]] inline size_t round_up_to_binary_power(int k)
{
  int n = 1;
  while (n < k) n <<= 1;
  return n;
}

enum class Direction
{
  Forward,
  Inverse,
};

template <Direction Dir> void transform(std::vector<cd>& a)
{
  size_t n = a.size();
  assert((n & (n - 1)) == 0 && "input size is not power of 2");

  auto rev   = reverse_bit(n);
  auto roots = unity_roots(n);

  for (size_t i = 0; i < n; i++)
    if (i < rev[i]) swap(a[i], a[rev[i]]);

  for (size_t len = 1; len < n; len <<= 1)
    for (size_t i = 0; i < n; i += 2 * len)
      for (size_t j = 0; j < len; j++)
      {
        cd u           = a[i + j];
        cd v           = a[i + j + len] * roots[len + j];
        a[i + j]       = u + v;
        a[i + j + len] = u - v;
      }

  if constexpr (Dir == Direction::Inverse)
  {
    std::reverse(a.begin() + 1, a.end());
    for (cd& x : a) x /= n;
  }
}

template <typename T>
[[nodiscard]] std::vector<cd> convolution(const std::vector<T>& a, const std::vector<T>& b)
{
  std::vector<cd> fa = fmap_cast<cd>(a);
  std::vector<cd> fb = fmap_cast<cd>(b);
  size_t n           = round_up_to_binary_power(a.size() + b.size());
  fa.resize(n);
  fb.resize(n);

  transform<Direction::Forward>(fa);
  transform<Direction::Forward>(fb);
  for (size_t i = 0; i < n; i++) fa[i] *= fb[i];
  transform<Direction::Inverse>(fa);

  fa.resize(a.size() + b.size() - 1);
  return fa;
}

template <typename T> [[nodiscard]] std::vector<cd> self_convolution(const std::vector<T>& a)
{
  std::vector<cd> fa = fmap_cast<cd>(a);
  size_t n           = round_up_to_binary_power(2 * a.size());
  fa.resize(n);

  transform<Direction::Forward>(fa);
  for (size_t i = 0; i < n; i++) fa[i] *= fa[i];
  transform<Direction::Inverse>(fa);

  fa.resize(2 * a.size() - 1);
  return fa;
}

} // namespace fft
