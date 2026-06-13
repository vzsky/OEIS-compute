#pragma once

#include <utils/maya/String.hpp>
#include <utils/maya/Tagged.hpp>

namespace maya {

constexpr auto format_tag          = "maya_format_str"_ms;
template <IsMayaStrT S> using FmtT = Tagged<S, format_tag>;
template <detail::CharCapture S> consteval auto operator""_f() noexcept { return FmtT<StrT<S>>{StrT<S>{}}; }

template <typename T>
concept IsMayaFormatT = requires(T t) { []<IsMayaStrT S>(Tagged<S, format_tag>) {}(t); };

static_assert(IsMayaStrT<decltype("x=$"_f)>);
static_assert(IsMayaFormatT<decltype("x=$"_f)>);
static_assert(!IsMayaFormatT<decltype("x=$"_ms)>);
static_assert(!IsMayaFormatT<int>);

} // namespace maya

using maya::operator""_f;
