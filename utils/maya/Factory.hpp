#pragma once

#include <array>
#include <cstddef>

namespace maya {

template<auto Keys, auto Fn>
struct Factory
{
  static constexpr std::size_t N = std::size(Keys);
  using key_type = std::remove_cvref_t<decltype(Keys[0])>;
  using value_type = std::remove_cvref_t<decltype(Fn(Keys[0]))>;

  static constexpr std::array<value_type, N> table = []() {
    std::array<value_type, N> t{};
    for (std::size_t i = 0; i < N; ++i) t[i] = Fn(Keys[i]);
    return t;
  }();

  static_assert([]() constexpr {
    for (std::size_t i = 0; i < N; ++i)
      for (std::size_t j = i + 1; j < N; ++j)
        if (Keys[i] == Keys[j]) return false;
    return true;
  }(), "Factory keys must be distinct");

  [[nodiscard]] static constexpr const value_type& find(const key_type& k)
  {
    for (std::size_t i = 0; i < N; ++i)
      if (Keys[i] == k) return table[i];
    __builtin_unreachable();
  }
};

} // namespace maya
