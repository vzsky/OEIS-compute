#pragma once

#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utils/maya/String.hpp>
#include <utils/maya/Tagged.hpp>

namespace format::detail {

template <maya::IsMayaStrT S> using FmtT = maya::Tagged<struct format_tag, S>;
template <maya::detail::CharCapture S> consteval auto operator""_f() noexcept
{
  return FmtT{maya::StrT<S>{}};
}

template <typename T>
concept IsMayaFormatT = requires(T t) { []<maya::IsMayaStrT S>(maya::Tagged<format_tag, S>) {}(t); };

static_assert(maya::IsMayaStrT<decltype("x=$"_f)>);
static_assert(IsMayaFormatT<decltype("x=$"_f)>);
static_assert(!IsMayaFormatT<decltype("x=$"_ms)>);
static_assert(!IsMayaFormatT<int>);

// TODO uint8_t shenanigan 
template <typename T>
concept PrintableRange = std::ranges::range<std::remove_cvref_t<T>> &&
                         !std::convertible_to<std::remove_cvref_t<T>, std::string_view>;

template <typename T> void print(std::ostream& os, T&& x) { os << std::forward<T>(x); }

template <PrintableRange T> void print(std::ostream& os, T&& x)
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
  print(os, std::forward<T>(x));
  print_format(os, s, pos + 1, std::forward<Ts>(xs)...);
}

template <std::size_t N, char sep = ' '> consteval auto args_fmt()
{
  if constexpr (N == 0)
    return FmtT<maya::StrT<"">>{{}};
  else
    return []<std::size_t... Is>(std::index_sequence<Is...>)
    {
      using S = maya::detail::String<(Is % 2 == 0 ? '$' : sep)...>;
      return FmtT<S>{S{}};
    }(std::make_index_sequence<2 * N - 1>{});
}

consteval std::size_t count_dollars(IsMayaFormatT auto fmt)
{
  std::size_t n = 0;
  for (char c : fmt.get().view()) n += (c == '$');
  return n;
}

} // namespace format::detail

using format::detail::operator""_f;

namespace format {

using format::detail::IsMayaFormatT;

template <IsMayaFormatT Fmt, typename... Ts> [[nodiscard]] std::string format(Fmt, Ts&&... xs)
{
  static_assert(detail::count_dollars(Fmt{}) == sizeof...(Ts),
                "format: wrong number of arguments for '$' placeholders");
  std::ostringstream oss;
  detail::print_format(oss, Fmt::Type::view(), 0, std::forward<Ts>(xs)...);
  return oss.str();
}

template <typename... Ts> [[nodiscard]] std::string format(Ts&&... xs)
{
  return format(detail::args_fmt<sizeof...(Ts)>(), std::forward<Ts>(xs)...);
}

} // namespace format
