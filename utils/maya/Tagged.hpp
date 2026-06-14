#pragma once

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

namespace maya {

template <typename Tag_, typename T> struct Tagged
{
  using Type = T;

  constexpr Tagged()
    requires std::default_initializable<Type>
  = default;

  constexpr explicit Tagged(Type val) : mVal(std::move(val)) {}

  constexpr Tagged& operator=(Type val) noexcept(std::is_nothrow_move_assignable_v<Type>)
  {
    mVal = std::move(val);
    return *this;
  }

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
struct _test_tag
{
};
using _DTag = maya::Tagged<_test_tag, double>;

static_assert(std::is_default_constructible_v<_DTag>);
static_assert(std::is_invocable_v<void (*)(double), _DTag>);
static_assert(!std::is_invocable_v<void (*)(_DTag), double>);
static_assert(std::is_convertible_v<maya::Tagged<_test_tag, int>, int>);
static_assert(std::is_convertible_v<maya::Tagged<_test_tag, std::string>, std::string>);
static_assert(!std::is_convertible_v<int, maya::Tagged<_test_tag, int>>);
static_assert(!std::is_convertible_v<std::string, maya::Tagged<_test_tag, std::string>>);
static_assert(std::is_convertible_v<maya::Tagged<_test_tag, int>, uint32_t>);
} // namespace
