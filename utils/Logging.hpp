#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <string_view>

namespace logging
{

namespace details
{

inline void print_module(std::string_view module)
{
  if (!module.empty()) std::cout << "[" << module << "] ";
}

template <typename... Ts> void log(std::string_view module, Ts&&... xs)
{
  print_module(module);
  ((std::cout << std::forward<Ts>(xs) << " "), ...);
  std::cout << std::endl;
}

inline void log_format_impl(std::string_view s, std::size_t i)
{
  if (s.find('$', i) != std::string_view::npos) throw std::invalid_argument("format/argument count mismatch");
  std::cout << s.substr(i);
}

template <typename T, typename... Ts>
void log_format_impl(std::string_view s, std::size_t i, T&& x, Ts&&... xs)
{
  auto pos = s.find('$', i);
  if (pos == std::string_view::npos) throw std::invalid_argument("format/argument count mismatch");
  std::cout << s.substr(i, pos - i);
  std::cout << std::forward<T>(x);
  log_format_impl(s, pos + 1, std::forward<Ts>(xs)...);
}

template <typename... Ts> void log_format(std::string_view module, std::string_view fmt, Ts&&... xs)
{
  print_module(module);
  log_format_impl(fmt, 0, std::forward<Ts>(xs)...);
  std::cout << std::endl;
}

} // namespace details

inline std::string print_time()
{
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%H:%M:%S}", now);
}

template <std::ranges::range R> std::string log_range(R&& range, std::string_view sep = " ")
{
  std::ostringstream oss;
  bool first = true;
  for (const auto& x : range)
  {
    if (!first) oss << sep;
    first = false;
    oss << x;
  }
  return oss.str();
}

} // namespace logging

inline constexpr std::string_view _LogModule_ = "";
#define Log(...) logging::details::log(_LogModule_, __VA_ARGS__)
#define LogF(fmt, ...) logging::details::log_format(_LogModule_, fmt, __VA_ARGS__)
#define LogModule(x) constexpr std::string_view _LogModule_ = x
