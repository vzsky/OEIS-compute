#pragma once

#include <type_traits>

int pow(int base, int k, int mod);
int pow(int base, int k); // can overflow

int gcd(int a, int b);
int lcm(int a, int b);

template <typename... Ts> constexpr int gcd(int a, int b, Ts... rest)
{
  static_assert((... && std::is_same_v<int, Ts>), "All arguments must be integers");
  return gcd(gcd(a, b), rest...);
}

template <typename... Ts> constexpr int lcm(int a, int b, Ts... rest)
{
  static_assert((... && std::is_same_v<int, Ts>), "All arguments must be integers");
  return lcm(lcm(a, b), rest...);
}
