#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <source_location>
#include <stdexcept>
#include <thread>
#include <vector>

namespace logging
{

namespace details
{

inline thread_local std::vector<std::string> env_stack;

struct EnvGuard
{
  EnvGuard(std::string s) { env_stack.push_back(std::move(s)); }
  ~EnvGuard() { env_stack.pop_back(); }
};

inline void print_env()
{
  for (auto& e : env_stack) std::cout << "[" << e << "] ";
}

template <typename... Ts> void log(Ts&&... xs)
{
  print_env();
  ((std::cout << std::forward<Ts>(xs) << " "), ...);
  std::cout << std::endl;
}

} // namespace details

inline std::string print_time()
{
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%H:%M:%S}", now);
}

template <std::ranges::range R> void print_range(const R& range, std::string sep = " ")
{
  for (auto x : range) std::cout << x << sep;
  std::cout << std::endl;
}

} // namespace logging

#define Log(...) logging::details::log(__VA_ARGS__)
#define PushLogScope(x)                                                                                      \
  logging::details::EnvGuard _log_env_guard_##__LINE__ { x }

namespace utils
{

template <typename Iterator, typename Predicate>
bool par_all_of(Iterator begin, Iterator end, Predicate pred,
                size_t max_threads = std::thread::hardware_concurrency())
{
  assert(max_threads > 0);
  const size_t length = std::distance(begin, end);
  if (length == 0) return true;
  if (length == 1) return pred(*begin);
  const size_t num_threads = std::min(length, max_threads);

  size_t block_size = (length + num_threads - 1) / num_threads;

  std::vector<std::future<bool>> futures;
  Iterator block_start = begin;

  for (size_t t = 0; t < num_threads && block_start != end; ++t)
  {
    Iterator block_end = block_start;
    size_t steps       = std::min(block_size, static_cast<size_t>(std::distance(block_start, end)));
    std::advance(block_end, steps);

    futures.push_back(std::async(std::launch::async, [block_start, block_end, &pred]()
    {
      for (auto it = block_start; it != block_end; ++it)
        if (!pred(*it)) return false;
      return true;
    }));

    block_start = block_end;
  }

  for (auto& f : futures)
  {
    if (!f.get()) return false;
  }

  return true;
}

template <typename T>
[[nodiscard]] inline std::vector<T>
read_bfile(const std::string& relative_path,
           const std::source_location& loc = std::source_location::current())
{
  namespace fs = std::filesystem;

  fs::path base = fs::path(loc.file_name()).parent_path();
  fs::path path = base / relative_path;

  std::ifstream in(path);
  if (!in) throw std::runtime_error("cannot open file");

  std::vector<T> out;

  std::string line;
  while (std::getline(in, line))
  {
    if (line.empty() || line[0] == '#') continue;

    std::istringstream iss(line);

    size_t n, a; // index and number, be careful of reading to int8_t (char)
    if (iss >> n >> a) out.push_back(a);
  }

  return out;
}

template <typename Func> auto timeit(const std::string& s, Func&& f)
{
  using namespace std::chrono;
  const auto start    = high_resolution_clock::now();
  const auto log_time = [&]
  {
    const auto us     = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
    const auto sec    = us / 1'000'000;
    const auto rem_us = us % 1'000'000;
    Log(s, std::format("-- Time elapsed: {}.{:06d} s", sec, rem_us));
  };

  if constexpr (std::is_void_v<std::invoke_result_t<Func>>)
  {
    f();
    log_time();
  }
  else
  {
    auto result = f();
    log_time();
    return result;
  }
}

template <typename Func> auto timeit(Func&& f) { return timeit("", f); }

} // namespace utils
