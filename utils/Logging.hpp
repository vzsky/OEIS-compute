#pragma once

#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utils/maya/StringFormat.hpp>
#include <vector>

namespace logging::detail {

enum class LogLevel
{
  Debug,
  Info,
  Warn,
  Error,
};

struct Logger
{
  void (*write)(std::string_view);
  void (*on_log_done)() = nullptr;
};

namespace print {

inline std::string time() { return std::format("{:%H:%M:%S}", std::chrono::system_clock::now()); }

inline void print_module(std::ostream& os, std::string_view module)
{
  if (!module.empty()) os << "[" << module << "] ";
}

template <typename T> void print_one(std::ostream& os, T&& x) { os << std::forward<T>(x); }

template <typename T>
concept PrintableRange = std::ranges::range<std::remove_cvref_t<T>> &&
                         !std::convertible_to<std::remove_cvref_t<T>, std::string_view>;

template <PrintableRange T> void print_one(std::ostream& os, T&& x)
{
  os << '{';
  bool first = true;
  for (const auto& e : x)
  {
    if (!first) os << ", ";
    first = false;
    os << e;
  }
  os << '}';
}

inline void print_format(std::ostream& os, std::string_view s, std::size_t i) { os << s.substr(i); }

template <typename T, typename... Ts>
void print_format(std::ostream& os, std::string_view s, std::size_t i, T&& x, Ts&&... xs)
{
  auto pos = s.find('$', i);
  os << s.substr(i, pos - i);
  print_one(os, std::forward<T>(x));
  print_format(os, s, pos + 1, std::forward<Ts>(xs)...);
}

} // namespace print

namespace loggers {

inline void normal_write(std::string_view msg) { std::cout << msg << '\n' << std::flush; }

inline void noop_write(std::string_view) {}

inline void timestamp_write(std::string_view msg)
{
  std::cout << logging::detail::print::time() << ' ' << msg << '\n' << std::flush;
}

inline void progress_write(std::string_view msg) { std::cout << '\r' << msg << "\033[K" << std::flush; }

inline void progress_done() { std::cout << '\n' << std::flush; }

inline constexpr Logger normal{&normal_write};
inline constexpr Logger noop{&noop_write};
inline constexpr Logger timestamp{&timestamp_write};
inline constexpr Logger progress{&progress_write, &progress_done};

} // namespace loggers

struct Env
{
  std::optional<std::string> mModule;
  std::optional<LogLevel> mLevel;
  std::optional<Logger> mLogger;

  Env module(std::string_view m) const
  {
    Env e     = *this;
    e.mModule = std::string(m);
    return e;
  }

  Env level(LogLevel l) const
  {
    Env e    = *this;
    e.mLevel = l;
    return e;
  }

  Env logger(Logger l) const
  {
    Env e     = *this;
    e.mLogger = l;
    return e;
  }

  Env operator+(const Env& delta) const
  {
    Env out = *this;

    if (delta.mModule)
    {
      if (mModule && !mModule->empty())
        out.mModule = *mModule + "::" + *delta.mModule;
      else
        out.mModule = delta.mModule;
    }

    if (delta.mLevel) out.mLevel = delta.mLevel;
    if (delta.mLogger) out.mLogger = delta.mLogger;
    return out;
  }
};

inline std::vector<Env>& env_stack()
{
  thread_local std::vector<Env> s{Env{std::string{}, LogLevel::Info, loggers::normal}};
  return s;
}

inline Env& current() { return detail::env_stack().back(); }

inline void emit_log(LogLevel level, std::string_view str)
{
  const Env& env = detail::current();
  assert(env.mLevel && env.mLogger && env.mModule);
  if (level < *env.mLevel) return;

  std::ostringstream oss;
  detail::print::print_module(oss, *env.mModule);
  oss << str;
  env.mLogger->write(oss.str());
}

} // namespace logging::detail

namespace logging {

using detail::Env;
using detail::Logger;
using detail::LogLevel;

inline Env snapshot() { return detail::current(); }

class Scope
{
public:
  Scope(const Env& delta)
  {
    detail::env_stack().push_back(detail::current() + delta);
    if (delta.mLogger) mOnExit = detail::current().mLogger->on_log_done;
  }

  ~Scope()
  {
    detail::env_stack().pop_back();
    if (mOnExit) mOnExit();
  }

  Scope(const Scope&)            = delete;
  Scope& operator=(const Scope&) = delete;

private:
  void (*mOnExit)() = nullptr;
};

} // namespace logging

namespace loggers {

using logging::detail::loggers::noop;
using logging::detail::loggers::normal;
using logging::detail::loggers::progress;
using logging::detail::loggers::timestamp;

} // namespace loggers

using LL = logging::detail::LogLevel;

template <maya::IsMayaFormat Fmt, typename... Ts> void Log(LL level, Fmt, Ts&&... xs)
{
  constexpr std::size_t dollars = []
  {
    std::size_t n = 0;
    for (char c : Fmt::view()) n += (c == '$');
    return n;
  }();
  static_assert(dollars == sizeof...(Ts));
  std::ostringstream oss;
  logging::detail::print::print_format(oss, Fmt::view(), 0, std::forward<Ts>(xs)...);
  logging::detail::emit_log(level, oss.str());
}

template <typename... Ts> void Log(LL level, Ts&&... xs)
{
  std::ostringstream oss;
  std::string_view sep;
  ((oss << sep, logging::detail::print::print_one(oss, std::forward<Ts>(xs)), sep = " "), ...);
  logging::detail::emit_log(level, oss.str());
}
