#pragma once

#include <type_traits>
#include <utils/maya/String.hpp>

namespace maya {

template <IsMayaStr Key, auto Value> struct DictEntry
{
  using key_t                 = Key;
  static constexpr auto value = Value;
};

namespace detail {

template <typename T>
concept IsEntry = requires { []<IsMayaStr K, auto V>(DictEntry<K, V>) {}(T{}); };

template <IsMayaStr Key, IsEntry... Entries> struct Lookup;

template <IsMayaStr Key, IsEntry First, IsEntry... Rest> struct Lookup<Key, First, Rest...>
{
  static constexpr auto value() noexcept
  {
    if constexpr (std::is_same_v<Key, typename First::key_t>)
      return First::value;
    else
      return Lookup<Key, Rest...>::value();
  }
};

template <IsMayaStr Key> struct Lookup<Key>
{
  static_assert(sizeof(Key) == 0, "Key not found in Dict");
};

} // namespace detail

// A compile-time dictionary keyed by MayaString.
// The dict IS its type
// Set prepends, so the most recently set value wins on lookup.
template <detail::IsEntry... Entries> struct Dict
{
  template <auto Key, auto Value>
    requires IsMayaStr<decltype(Key)>
  using Set = Dict<DictEntry<decltype(Key), Value>, Entries...>;

  template <auto Key>
    requires IsMayaStr<decltype(Key)>
  static constexpr auto get = detail::Lookup<decltype(Key), Entries...>::value();

  template <auto Key>
    requires IsMayaStr<decltype(Key)>
  static constexpr bool contains = (std::is_same_v<decltype(Key), typename Entries::key_t> || ...);
};

// ---- spec / documentation ----

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
