#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

template <size_t Cap> struct RingBuffer
{
  void push(uint64_t val) { mBuf[mCount % Cap] = val; ++mCount; }

  [[nodiscard]] uint64_t operator[](size_t age) const { return mBuf[(mCount - 1 - age) % Cap]; }
  [[nodiscard]] bool     has(size_t age)         const { return mCount > age; }

private:
  std::array<uint64_t, Cap> mBuf{};
  size_t                    mCount = 0;
};
