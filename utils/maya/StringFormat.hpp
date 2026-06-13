#pragma once

#include <utils/maya/String.hpp>

namespace maya {

template <detail::CharCapture S>
using FmtT = decltype([]<std::size_t... Is>(std::index_sequence<Is...>)
{ return detail::FormatString<S.value[Is]...>{}; }(std::make_index_sequence<sizeof(S.value) - 1>{}));

template <detail::CharCapture S> consteval auto operator""_f() noexcept { return FmtT<S>{}; }

template <typename T>
concept IsMayaFormat = requires { []<char... Cs>(detail::FormatString<Cs...>) {}(T{}); };

static_assert(IsMayaStr<decltype("test"_f)>);
static_assert(IsMayaFormat<decltype("test"_f)>);
static_assert(!IsMayaFormat<decltype("test"_ms)>);

} // namespace maya

using maya::operator""_f;
