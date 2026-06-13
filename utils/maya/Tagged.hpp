#pragma once

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <utils/maya/String.hpp>

namespace maya {

template <typename T_, IsMayaStrT auto Tag_> struct Tagged
{
  using Type = T_;

  constexpr Tagged()
    requires std::default_initializable<Type>
  = default;
  constexpr explicit Tagged(Type&& val) : mVal(std::move(val)) {}

  constexpr operator const Type&() const noexcept { return mVal; }
  constexpr operator Type&() noexcept { return mVal; }

  constexpr const Type& get() const noexcept { return mVal; }
  constexpr Type& get() noexcept { return mVal; }

  auto operator<=>(const Tagged&) const = default;

  friend std::ostream& operator<<(std::ostream& os, const Tagged& t) { return os << t.mVal; }

private:
  Type mVal;
};

} // namespace maya

namespace {
using _DTag = maya::Tagged<double, "test"_ms>;

static_assert(std::is_default_constructible_v<_DTag>);
static_assert(std::is_invocable_v<void (*)(double), _DTag>);
static_assert(!std::is_invocable_v<void (*)(_DTag), double>);
static_assert(std::is_convertible_v<maya::Tagged<int, "test"_ms>, int>);
static_assert(std::is_convertible_v<maya::Tagged<std::string, "test"_ms>, std::string>);
static_assert(!std::is_convertible_v<int, maya::Tagged<int, "test"_ms>>);
static_assert(!std::is_convertible_v<std::string, maya::Tagged<std::string, "test"_ms>>);
static_assert(std::is_convertible_v<maya::Tagged<int, "test"_ms>, uint32_t>);
} // namespace
