#pragma once

#include <algorithm>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace maya::detail {

template <char... Cs> struct String
{
  static constexpr char buf[] = {Cs..., '\0'};
  static constexpr std::string_view view() noexcept { return {buf, sizeof...(Cs)}; }
  static constexpr bool empty() noexcept { return sizeof...(Cs) == 0; }

  constexpr operator std::string_view() const noexcept { return view(); }
  friend std::ostream& operator<<(std::ostream& os, String) { return os << view(); }
};

template <char... Cs1, char... Cs2> constexpr bool operator==(String<Cs1...>, String<Cs2...>) noexcept
{
  return std::is_same_v<String<Cs1...>, String<Cs2...>>;
}

template <char... Cs1, char... Cs2> constexpr auto operator+(String<Cs1...>, String<Cs2...>) noexcept
{
  return String<Cs1..., Cs2...>{};
}

template <std::size_t N> struct CharCapture
{
  char value[N];
  constexpr CharCapture(const char (&s)[N]) noexcept { std::copy_n(s, N, value); }
};
template <std::size_t N> CharCapture(const char (&)[N]) -> CharCapture<N>;

template <maya::detail::CharCapture S> consteval auto mayaStr() noexcept
{
  return []<std::size_t... Is>(std::index_sequence<Is...>)
  { return maya::detail::String<S.value[Is]...>{}; }(std::make_index_sequence<sizeof(S.value) - 1>{});
}

} // namespace maya::detail

namespace maya {

using maya::detail::String;

template <maya::detail::CharCapture S> using StrT = decltype(maya::detail::mayaStr<S>());

template <maya::detail::CharCapture S> consteval auto operator""_ms() noexcept { return StrT<S>{}; }

template <typename T>
concept IsMayaStr = requires { []<char... Cs>(maya::detail::String<Cs...>) {}(T{}); };

using emptyStrT = decltype(""_ms);

static_assert(IsMayaStr<emptyStrT>);
static_assert(IsMayaStr<decltype("test"_ms)>);
static_assert("test"_ms.view() == "test");
static_assert("ab"_ms == "ab"_ms);
static_assert("ab"_ms != "cd"_ms);
static_assert(("hello"_ms + " world"_ms) == "hello world"_ms);
static_assert(""_ms.empty());
static_assert(!"a"_ms.empty());

} // namespace maya
