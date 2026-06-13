#pragma once

#include <algorithm>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace maya::detail {

struct MayaStrBase
{
};

template <char... Cs> struct String : MayaStrBase
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
concept IsPureMayaStrT = requires { []<char... Cs>(maya::detail::String<Cs...>) {}(T{}); };

template <auto V>
concept IsPureMayaStr = IsPureMayaStrT<decltype(V)>;

template <typename T>
concept IsMayaStrT = std::is_convertible_v<T, maya::detail::MayaStrBase>;

template <auto V>
concept IsMayaStr = IsMayaStrT<decltype(V)>;

static_assert(IsPureMayaStrT<StrT<"">>);
static_assert(IsPureMayaStrT<decltype("test"_ms)>);
static_assert(IsPureMayaStr<""_ms>);
static_assert(IsPureMayaStr<"test"_ms>);
static_assert(IsMayaStrT<StrT<"">>);
static_assert(IsMayaStr<""_ms>);
static_assert("test"_ms.view() == "test");
static_assert(StrT<"sth">::view() == "sth");
static_assert("ab"_ms == "ab"_ms);
static_assert("ab"_ms != "cd"_ms);
static_assert(("hello"_ms + " world"_ms) == "hello world"_ms);
static_assert(""_ms.empty());
static_assert(!"a"_ms.empty());

} // namespace maya

using maya::operator""_ms;
