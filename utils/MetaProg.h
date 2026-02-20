#include <type_traits>

namespace mp
{

template <int I, int N, typename F> constexpr void For(F&& f)
{
  static_assert(std::is_integral_v<int>);

  if constexpr (I < N)
  {
    f(std::integral_constant<int, I>{});
    For<int(I + 1), N>(f);
  }
}

} // namespace mp
