#pragma once

#include <utils/maya/String.hpp>

namespace maya {

template <IsMayaStrT auto Key, auto Value> struct DictEntry
{
  static constexpr auto key   = Key;
  static constexpr auto value = Value;
};

namespace detail {

template <typename T>
concept IsEntry = requires { []<IsMayaStrT auto K, auto V>(DictEntry<K, V>) {}(T{}); };

template <IsMayaStrT auto Key, IsEntry... Entries> struct Lookup;
template <IsMayaStrT auto Key, IsEntry First, IsEntry... Rest> struct Lookup<Key, First, Rest...>
{
  static constexpr auto value() noexcept
  {
    if constexpr (First::key == Key)
      return First::value;
    else
      return Lookup<Key, Rest...>::value();
  }
};

template <IsMayaStrT auto Key> struct Lookup<Key>
{
  static_assert(false, "Key not found in Dict");
};

} // namespace detail

template <detail::IsEntry... Entries> struct Dict
{
  template <IsMayaStrT auto Key, auto Value> using Set = Dict<DictEntry<Key, Value>, Entries...>;

  template <IsMayaStrT auto Key> static constexpr auto get = detail::Lookup<Key, Entries...>::value();

  template <IsMayaStrT auto Key> static constexpr bool contains = ((Entries::key == Key) || ...);
};

using D0 = maya::Dict<>;
static_assert(!D0::contains<"x"_ms>);

using D1 = D0::Set<"x"_ms, 42>::Set<"y"_ms, 99>;
static_assert(D1::get<"x"_ms> == 42);
static_assert(D1::get<"y"_ms> == 99);
static_assert(D1::contains<"x"_ms>);
static_assert(D1::contains<"y"_ms>);
static_assert(!D1::contains<"z"_ms>);

using D2 = D1::Set<"x"_ms, 7>;
static_assert(D2::get<"x"_ms> == 7);
static_assert(D2::get<"y"_ms> == 99);

using D3 = D2::Set<"flag"_ms, true>::Set<"pi"_ms, 3.14>;
static_assert(D3::get<"flag"_ms> == true);
static_assert(D3::get<"pi"_ms> == 3.14);

using D4 = D3::Set<"flag"_ms, "what"_ms>::Set<"pi"_ms, "3.14"_ms>;
static_assert(D4::get<"flag"_ms> == "what"_ms);
static_assert(D4::get<"pi"_ms> == "3.14"_ms);

} // namespace maya
