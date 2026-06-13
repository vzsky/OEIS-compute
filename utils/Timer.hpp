#pragma once

#include <chrono>
#include <string>

#include <utils/Logging.hpp>

namespace utils::timer {

struct [[nodiscard]] ScopeTimer
{
  explicit ScopeTimer(std::string label = "")
      : mLabel(std::move(label)), mStart(std::chrono::high_resolution_clock::now())
  {
  }

  ~ScopeTimer()
  {
    using namespace std::chrono;
    const auto us     = duration_cast<microseconds>(high_resolution_clock::now() - mStart).count();
    const auto sec    = us / 1'000'000;
    const auto rem_us = us % 1'000'000;
    Log(LL::Info, mLabel, "-- Time elapsed:", std::format("{}.{:06d} s", sec, rem_us));
  }

  ScopeTimer(ScopeTimer&&)                 = delete;
  ScopeTimer& operator=(ScopeTimer&&)      = delete;
  ScopeTimer(const ScopeTimer&)            = delete;
  ScopeTimer& operator=(const ScopeTimer&) = delete;

private:
  std::string mLabel;
  std::chrono::high_resolution_clock::time_point mStart;
};

} // namespace utils::timer
