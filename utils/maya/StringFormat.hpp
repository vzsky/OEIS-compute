#pragma once

#include <utils/maya/String.hpp>
#include <utils/maya/Tagged.hpp>

namespace maya {

template <IsMayaStrT S> using FmtT = Tagged<struct format_tag, S>;
template <detail::CharCapture S> consteval auto operator""_f() noexcept { return FmtT<StrT<S>>{StrT<S>{}}; }

template <typename T>
concept IsMayaFormatT = requires(T t) { []<IsMayaStrT S>(Tagged<format_tag, S>) {}(t); };

static_assert(IsMayaStrT<decltype("x=$"_f)>);
static_assert(IsMayaFormatT<decltype("x=$"_f)>);
static_assert(!IsMayaFormatT<decltype("x=$"_ms)>);
static_assert(!IsMayaFormatT<int>);

} // namespace maya

using maya::operator""_f;
