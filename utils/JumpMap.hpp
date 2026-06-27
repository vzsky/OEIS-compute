#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>

namespace utils {

template <auto Keys, typename ValueT>
  requires(Keys.size() > 0 && std::integral<typename decltype(Keys)::value_type>)
class JumpMap
{
  using KeyT                            = typename decltype(Keys)::value_type;
  static constexpr std::size_t kN       = Keys.size();
  static constexpr KeyT        kMin     = *std::min_element(Keys.begin(), Keys.end());
  static constexpr KeyT        kMax     = *std::max_element(Keys.begin(), Keys.end());
  static constexpr std::size_t kRange   = static_cast<std::size_t>(kMax - kMin) + 1;
  static constexpr std::size_t kInvalid = kN;

  static constexpr auto buildTable() noexcept
  {
    std::array<std::size_t, kRange> t{};
    t.fill(kInvalid);
    for (std::size_t i = 0; i < kN; ++i)
      t[static_cast<std::size_t>(Keys[i] - kMin)] = i;
    return t;
  }
  static constexpr auto kTable = buildTable();

  std::array<ValueT, kN> mData{};

public:
  template <KeyT Key> [[nodiscard]] ValueT& get() noexcept
  {
    constexpr std::size_t idx = kTable[static_cast<std::size_t>(Key - kMin)];
    static_assert(idx != kInvalid, "Key not present in JumpMap");
    return mData[idx];
  }

  template <KeyT Key> [[nodiscard]] const ValueT& get() const noexcept
  {
    constexpr std::size_t idx = kTable[static_cast<std::size_t>(Key - kMin)];
    static_assert(idx != kInvalid, "Key not present in JumpMap");
    return mData[idx];
  }

  [[nodiscard]] ValueT&       operator[](KeyT k) noexcept { return mData[kTable[static_cast<std::size_t>(k - kMin)]]; }
  [[nodiscard]] const ValueT& operator[](KeyT k) const noexcept { return mData[kTable[static_cast<std::size_t>(k - kMin)]]; }

  void fill(const ValueT& v) noexcept { mData.fill(v); }

  [[nodiscard]] static constexpr std::size_t size() noexcept { return kN; }

  [[nodiscard]] ValueT*       data() noexcept { return mData.data(); }
  [[nodiscard]] const ValueT* data() const noexcept { return mData.data(); }
};

} // namespace utils
