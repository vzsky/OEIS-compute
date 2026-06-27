#pragma once

#include <cstddef>
#include <functional>
#include <utility>

namespace hash {

template <typename T> void add(size_t& seed, const T& v)
{
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename... Ts> [[nodiscard]] size_t combine(const Ts&... vs)
{
  size_t seed = 0;
  (add(seed, vs), ...);
  return seed;
}

template <typename T> struct Hasher
{
  [[nodiscard]] size_t operator()(const T& v) const { return v.hash(); }
};

template <typename A, typename B> struct PairHasher
{
  [[nodiscard]] size_t operator()(const std::pair<A, B>& v) const { return combine(v.first, v.second); }
};

} // namespace hash

template <typename A, typename B> struct std::hash<std::pair<A, B>>
{
  [[nodiscard]] size_t operator()(const std::pair<A, B>& v) const
  {
    return ::hash::combine(v.first, v.second);
  }
};
