#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <future>
#include <iterator>
#include <mutex>
#include <optional>
#include <thread>
#include <utils/Logging.hpp>
#include <vector>

namespace utils::parallel {

struct MonitorConfig
{
  int intervalSeconds;
  bool enabled = true;
};

inline constexpr MonitorConfig noMonitor{
    .intervalSeconds = 0,
    .enabled         = false,
};

namespace detail {

struct ProgressMonitor
{
  ProgressMonitor(const std::atomic<size_t>& done, size_t length, MonitorConfig conf)
      : mFinished(false), mFuture(std::async(std::launch::async, [&done, this, length, conf]()
  {
    logging::Scope _l = logging::Env{}.logger(loggers::progress);
    while (!mFinished.load(std::memory_order_relaxed))
    {
      std::this_thread::sleep_for(std::chrono::seconds(conf.intervalSeconds));
      size_t d = done.load(std::memory_order_relaxed);
      if (!mFinished.load(std::memory_order_relaxed))
        Log(LL::Info, "progress: $/$ ($%)"_f, d, length, 100 * d / length);
    }
  }))
  {
  }

  ~ProgressMonitor()
  {
    mFinished.store(true);
    mFuture.wait();
  }

  ProgressMonitor(const ProgressMonitor&)            = delete;
  ProgressMonitor& operator=(const ProgressMonitor&) = delete;

private:
  std::atomic<bool> mFinished;
  std::future<void> mFuture;
};

} // namespace detail

// Need to assume func to be called in any order!
template <MonitorConfig monConf = noMonitor, typename Iterator, typename Func>
void foreach (Iterator begin, Iterator end, Func func,
              size_t max_threads = std::thread::hardware_concurrency())
{
  assert(max_threads > 0);
  const size_t length = std::distance(begin, end);
  if (length == 0) return;
  const size_t num_threads = std::min(length, max_threads);

  std::atomic<size_t> next{0};
  [[maybe_unused]] std::atomic<size_t> done{0};

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);

  for (size_t t = 0; t < num_threads; ++t)
  {
    futures.push_back(std::async(std::launch::async, [&]()
    {
      size_t i;
      while ((i = next.fetch_add(1, std::memory_order_relaxed)) < length)
      {
        func(*std::next(begin, i));
        if constexpr (monConf.enabled) done.fetch_add(1, std::memory_order_relaxed);
      }
    }));
  }

  std::optional<detail::ProgressMonitor> monitor;
  if constexpr (monConf.enabled) monitor.emplace(done, length, monConf);

  for (auto& f : futures) f.get();
}

template <MonitorConfig monConf = noMonitor, typename Iterator, typename Predicate>
[[nodiscard]] bool all_of(Iterator begin, Iterator end, Predicate pred,
                          size_t max_threads = std::thread::hardware_concurrency())
{
  assert(max_threads > 0);
  const size_t length = std::distance(begin, end);
  if (length == 0) return true;
  const size_t num_threads = std::min(length, max_threads);

  std::atomic<size_t> next{0};
  std::atomic<bool> failed{false};
  [[maybe_unused]] std::atomic<size_t> done{0};

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);

  for (size_t t = 0; t < num_threads; ++t)
  {
    futures.push_back(std::async(std::launch::async, [&]()
    {
      size_t i;
      while (!failed.load(std::memory_order_relaxed) &&
             (i = next.fetch_add(1, std::memory_order_relaxed)) < length)
      {
        if (!pred(*std::next(begin, i)))
        {
          failed.store(true, std::memory_order_relaxed);
          return;
        }
        if constexpr (monConf.enabled) done.fetch_add(1, std::memory_order_relaxed);
      }
    }));
  }

  std::optional<detail::ProgressMonitor> monitor;
  if constexpr (monConf.enabled) monitor.emplace(done, length, monConf);

  for (auto& f : futures) f.get();
  return !failed.load();
}

// Result order is NOT guaranteed.
template <MonitorConfig monConf = noMonitor, typename Iterator, typename Predicate>
[[nodiscard]] auto filter(Iterator begin, Iterator end, Predicate pred,
                          size_t max_threads = std::thread::hardware_concurrency())
{
  using T = typename std::iterator_traits<Iterator>::value_type;

  std::mutex mtx;
  std::vector<T> result;
  foreach
    <monConf>(begin, end, [&](const T& elem)
    {
      if (pred(elem))
      {
        std::lock_guard lock(mtx);
        result.push_back(elem);
      }
    }, max_threads);
  return result;
}

template <MonitorConfig monConf = noMonitor, typename Range, typename Predicate>
[[nodiscard]] bool all_of(Range&& r, Predicate pred, size_t max_threads = std::thread::hardware_concurrency())
{
  return all_of<monConf>(std::begin(r), std::end(r), pred, max_threads);
}

template <MonitorConfig monConf = noMonitor, typename Range, typename Predicate>
[[nodiscard]] auto filter(Range&& r, Predicate pred, size_t max_threads = std::thread::hardware_concurrency())
{
  return filter<monConf>(std::begin(r), std::end(r), pred, max_threads);
}

template <MonitorConfig monConf = noMonitor, typename Range, typename Func>
void foreach (Range&& r, Func func, size_t max_threads = std::thread::hardware_concurrency())
{
  foreach
    <monConf>(std::begin(r), std::end(r), func, max_threads);
}

} // namespace utils::parallel
