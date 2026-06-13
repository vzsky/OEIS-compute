#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <sstream>
#include <string_view>
#include <utils/maya/Dictionary.hpp>
#include <utils/maya/String.hpp>

namespace logging::detail {

using namespace maya;

enum class LogLevel
{
  Debug,
  Info,
  Warn,
  Error,
};

using Logger = void (*)(std::string_view);

template <IsMayaStr NameT, LogLevel level_, Logger logger_>
struct LogEnv : maya::Dict<                                        //
                    maya::DictEntry<decltype("name"_ms), NameT{}>, //
                    maya::DictEntry<decltype("level"_ms), level_>  //
                    >
{
  static constexpr auto name      = NameT{};
  static constexpr LogLevel level = level_;
  static constexpr Logger logger  = logger_;

  // for `using _Env_ = _Env_::Module<"module">`
  template <maya::detail::CharCapture S>
  using Module = LogEnv<                                                 //
      std::conditional_t<NameT::empty(),                                 //
                         maya::StrT<S>,                                  //
                         decltype(NameT{} + "::"_ms + maya::StrT<S>{})>, //
      level, logger>;

  // for `using _Env_ = _Env_::Level<newL>`
  template <LogLevel newLevel> using Level = LogEnv<NameT, newLevel, logger>;

  // for `using _Env_ = _Env_::Logger<loggers::progress>`
  template <Logger newLogger> using Logger = LogEnv<NameT, level, newLogger>;
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
  for (const auto& elem : x)
  {
    if (!first) os << ", ";
    first = false;
    os << elem;
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

template <typename T>
concept IsLogEnv = requires {
  requires IsMayaStr<decltype(T::name)>;
  requires std::same_as<std::remove_cv_t<decltype(T::level)>, LogLevel>;
  requires std::same_as<std::remove_cv_t<decltype(T::logger)>, Logger>;
};

template <LogLevel MsgLevel, IsLogEnv Mod, typename... Ts> void log(Mod, Ts&&... xs)
{
  if constexpr (MsgLevel >= Mod::level)
  {
    std::ostringstream oss;
    print::print_module(oss, Mod::name);
    ((print::print_one(oss, std::forward<Ts>(xs)), oss << " "), ...);
    Mod::logger(oss.str());
  }
}

template <LogLevel MsgLevel, IsLogEnv Mod, IsMayaStr Fmt, typename... Ts>
void log_format(Mod, Fmt, Ts&&... xs)
{
  constexpr std::size_t dollars = []
  {
    std::size_t n = 0;
    for (char c : Fmt::view()) n += (c == '$');
    return n;
  }();
  static_assert(dollars == sizeof...(Ts), "LogF: number of $ placeholders must match argument count");
  if constexpr (MsgLevel >= Mod::level)
  {
    std::ostringstream oss;
    print::print_module(oss, Mod::name);
    print::print_format(oss, Fmt::view(), 0, std::forward<Ts>(xs)...);
    Mod::logger(oss.str());
  }
}

} // namespace logging::detail

using maya::operator""_ms;
using LL = logging::detail::LogLevel;

namespace loggers {

inline void noop(std::string_view) {}

inline void timestamp(std::string_view msg)
{
  std::string line = logging::detail::print::time();
  line += ' ';
  line += msg;
  line += '\n';
  std::cout << line;
  std::cout.flush();
}

inline void normal(std::string_view msg)
{
  std::string line(msg);
  line += '\n';
  std::cout << line;
  std::cout.flush();
}

inline void progress(std::string_view msg)
{
  std::string line = "\r";
  line += msg;
  line += "\033[K";
  std::cerr << line;
  std::cerr.flush();
}

} // namespace loggers

using _LogEnv_ = logging::detail::LogEnv<maya::emptyStrT, LL::Info, loggers::normal>;
#define Log(lvl, ...) logging::detail::log<logging::detail::LogLevel::lvl>(_LogEnv_{}, __VA_ARGS__)
#define LogF(lvl, fmt, ...)                                                                                  \
  logging::detail::log_format<logging::detail::LogLevel::lvl>(_LogEnv_{}, maya::StrT<fmt>{}, __VA_ARGS__)
