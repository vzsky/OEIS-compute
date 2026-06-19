#pragma once

#include <atomic>
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utils/Format.hpp>
#include <vector>

namespace logging::detail {

enum class LogLevel
{
  Infra,
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

} // namespace print

} // namespace logging::detail

namespace logging::detail {

namespace loggers {

inline void normal_write(std::string_view msg) { std::cout << msg << '\n' << std::flush; }

inline void noop_write(std::string_view) {}

inline void timestamp_write(std::string_view msg)
{
  std::cout << logging::detail::print::time() << ' ' << msg << '\n' << std::flush;
}

inline void progress_write(std::string_view msg) { std::cout << '\r' << msg << "\033[K" << std::flush; }

inline void progress_done() { std::cout << '\n' << std::flush; }

template <size_t ThrottleFreq> struct Throttler
{
  inline static std::atomic<size_t> count{0};
};

template <size_t ThrottleFreq> void throttled_normal_write(std::string_view msg)
{
  using S = Throttler<ThrottleFreq>;
  if ((S::count.fetch_add(1, std::memory_order_relaxed) + 1) % ThrottleFreq == 0)
    std::cout << msg << '\n' << std::flush;
}

template <size_t ThrottleFreq> void throttled_progress_write(std::string_view msg)
{
  using S           = Throttler<ThrottleFreq>;
  bool should_print = (S::count.fetch_add(1, std::memory_order_relaxed) + 1) % ThrottleFreq == 0;
  if (should_print) std::cout << '\r' << msg << "\033[K" << std::flush;
}

template <size_t ThrottleFreq> void throttled_progress_done() { std::cout << '\n' << std::flush; }

inline constexpr Logger normal{&normal_write};
inline constexpr Logger noop{&noop_write};
inline constexpr Logger timestamp{&timestamp_write};
inline constexpr Logger progress{&progress_write, &progress_done};

template <size_t ThrottleFreq> inline constexpr Logger throttled{&throttled_normal_write<ThrottleFreq>};
template <size_t ThrottleFreq>
inline constexpr Logger throttled_progress{&throttled_progress_write<ThrottleFreq>,
                                           &throttled_progress_done<ThrottleFreq>};

} // namespace loggers

struct Env
{
  std::optional<std::string> mModule;
  std::optional<LogLevel> mLevel;
  std::optional<Logger> mLogger;

  [[nodiscard]] Env module(std::string_view m) const
  {
    Env e     = *this;
    e.mModule = std::string(m);
    return e;
  }

  [[nodiscard]] Env level(LogLevel l) const
  {
    Env e    = *this;
    e.mLevel = l;
    return e;
  }

  [[nodiscard]] Env logger(Logger l) const
  {
    Env e     = *this;
    e.mLogger = l;
    return e;
  }

  [[nodiscard]] Env operator+(const Env& delta) const
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

[[nodiscard]] inline Env snapshot() { return detail::current(); }

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
using logging::detail::loggers::throttled;
using logging::detail::loggers::throttled_progress;
using logging::detail::loggers::timestamp;

} // namespace loggers

using LL = logging::detail::LogLevel;

template <format::IsMayaFormatT Fmt, typename... Ts> void Log(LL level, Fmt fmt, Ts&&... xs)
{
  logging::detail::emit_log(level, format::format(fmt, std::forward<Ts>(xs)...));
}

template <typename... Ts> void Log(LL level, Ts&&... xs)
{
  logging::detail::emit_log(level, format::format(std::forward<Ts>(xs)...));
}
