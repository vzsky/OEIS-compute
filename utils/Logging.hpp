#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <vector>

namespace logging
{

namespace details
{

inline thread_local std::vector<std::string> env_stack;

struct EnvGuard
{
  EnvGuard(std::string s) { env_stack.push_back(std::move(s)); }
  ~EnvGuard() { env_stack.pop_back(); }
};

inline void print_env()
{
  for (auto& e : env_stack) std::cout << "[" << e << "] ";
}

template <typename... Ts> void log(Ts&&... xs)
{
  print_env();
  ((std::cout << std::forward<Ts>(xs) << " "), ...);
  std::cout << std::endl;
}

inline void log_format_impl(const std::string& s, std::size_t i)
{
  if (s.find('$', i) != std::string::npos) throw std::invalid_argument("format/argument count mismatch");
  std::cout << s.substr(i);
}

template <typename T, typename... Ts>
void log_format_impl(const std::string& s, std::size_t i, T&& x, Ts&&... xs)
{
  auto pos = s.find('$', i);
  if (pos == std::string::npos) throw std::invalid_argument("format/argument count mismatch");

  std::cout << s.substr(i, pos - i);
  std::cout << std::forward<T>(x);
  log_format_impl(s, pos + 1, std::forward<Ts>(xs)...);
}

template <typename... Ts> void log_format(std::string s, Ts&&... xs)
{
  print_env();
  log_format_impl(s, 0, std::forward<Ts>(xs)...);
  std::cout << '\n';
}

} // namespace details

inline std::string print_time()
{
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%H:%M:%S}", now);
}

template <std::ranges::range R> std::string log_range(const R& range, std::string_view sep = " ")
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

#define Log(...) logging::details::log(__VA_ARGS__)
#define LogF(...) logging::details::log_format(__VA_ARGS__)
#define PushLogScope(x)                                                                                      \
  logging::details::EnvGuard _log_env_guard_##__LINE__ { x }


