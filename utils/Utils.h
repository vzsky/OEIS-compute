#include <algorithm>
#include <cassert>
#include <chrono>
#include <future>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

namespace utils
{

template <typename Iterator, typename Predicate>
bool par_all_of(Iterator begin, Iterator end, Predicate pred,
                size_t max_threads = std::thread::hardware_concurrency())
{
  assert(max_threads > 0);
  const size_t length = std::distance(begin, end);
  if (length == 0)
    return true;
  const size_t num_threads = std::min(length, max_threads);

  size_t block_size = (length + num_threads - 1) / num_threads;

  std::vector<std::future<bool>> futures;
  Iterator block_start = begin;

  for (size_t t = 0; t < num_threads && block_start != end; ++t)
  {
    Iterator block_end = block_start;
    size_t steps = std::min(block_size, static_cast<size_t>(std::distance(block_start, end)));
    std::advance(block_end, steps);

    futures.push_back(std::async(std::launch::async,
                                 [block_start, block_end, &pred]()
                                 {
                                   for (auto it = block_start; it != block_end; ++it)
                                     if (!pred(*it))
                                       return false;
                                   return true;
                                 }));

    block_start = block_end;
  }

  for (auto& f : futures)
  {
    if (!f.get())
      return false;
  }

  return true;
}

template <typename Func> auto timeit(Func f)
{
  using namespace std::chrono;
  auto start = high_resolution_clock::now();

  f();

  auto end = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start).count();
  std::cout << "Time elapsed: " << duration / 1000 << "." << duration % 1000 << "s" << std::endl;
}

} // namespace utils
