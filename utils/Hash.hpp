#pragma once

#include <cstddef>
#include <functional>

namespace hash
{

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

} // namespace hash
