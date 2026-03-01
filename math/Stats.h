#pragma once

#include <numeric>
#include <ranges>

namespace stats
{

template <std::ranges::input_range R> double average(R&& r)
{
  size_t n   = std::ranges::distance(r);
  double sum = std::accumulate(std::begin(r), std::end(r), 0.0);
  return sum / n;
}

template <std::ranges::input_range R> double variance(R&& r)
{
  size_t n = std::ranges::distance(r);
  if (n < 2) return 0.0;

  double mean      = average(r);
  auto transformed = r                                                              //
                     | std::views::transform([mean](double x) { return x - mean; }) //
                     | std::views::transform([](double x) { return x * x; });       //

  double sq_sum = std::reduce(std::begin(transformed), std::end(transformed), 0.0, std::plus<>());
  return sq_sum / n;
}

} // namespace stats
