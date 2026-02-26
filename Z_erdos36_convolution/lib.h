#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
#include <vector>

constexpr double DOUBLE_COMPARE_EPS = 1e-6;
inline bool feq(double a, double b) { return abs(a - b) < DOUBLE_COMPARE_EPS; }
inline bool fgt(double a, double b) { return a - b > DOUBLE_COMPARE_EPS; }

namespace step_func
{

// piecewise step defined on [0, 1)
template <typename Impl, std::size_t N> class PiecewiseStepView
{
public:
  // returns f( [i/N .. i+1/N) )
  double area() const { return static_cast<const Impl*>(this)->area(); }
  double operator[](std::size_t i) const { return static_cast<const Impl*>(this)->operator[](i); }
};

template <std::size_t N> class PiecewiseStep : public PiecewiseStepView<PiecewiseStep<N>, N>
{
  // height of i/N to i+1/N
  std::array<double, N> heights;

public:
  PiecewiseStep(const std::array<double, N>& h) : heights{h} {}
  PiecewiseStep(std::array<double, N>&& h) : heights{std::move(h)} {}

  constexpr PiecewiseStep(const double (&h)[N]) : heights{}
  {
    for (std::size_t i = 0; i < N; i++) heights[i] = h[i];
  }
  template <typename T> constexpr PiecewiseStep(const PiecewiseStepView<T, N>& h) : heights{}
  {
    for (std::size_t i = 0; i < N; i++) heights[i] = h[i];
  }

  const std::array<double, N>& get_heights() const { return heights; }
  double area() const { return std::accumulate(heights.begin(), heights.end(), 0.0) / N; }
  double operator[](std::size_t i) const { return heights[i]; }
};

template <typename T, std::size_t N> class OneMinusView : public PiecewiseStepView<OneMinusView<T, N>, N>
{
  const PiecewiseStepView<T, N>& source;

public:
  OneMinusView(const PiecewiseStepView<T, N>& f) : source(f) {}
  double area() const { return 1.0 - source.area(); }
  double operator[](std::size_t i) const { return 1.0 - source[i]; }
};

template <typename T, std::size_t N> OneMinusView<T, N> one_minus(const PiecewiseStepView<T, N>& f)
{
  return OneMinusView<T, N>(f);
}

// mirrow the shape but retains [0, 1]
template <typename T, std::size_t N> class MirrorView : public PiecewiseStepView<MirrorView<T, N>, 2 * N - 1>
{
  const PiecewiseStepView<T, N>& source;

public:
  MirrorView(const PiecewiseStepView<T, N>& f) : source(f) {}
  double area() const { return source.area() * (2 * N) / (2 * N - 1) - source[N - 1] / (2 * N - 1); }
  double operator[](std::size_t i) const
  {
    if (i >= N) return source[2 * N - 2 - i];
    return source[i];
  }
};

template <typename T, std::size_t N> MirrorView<T, N> mirror(const PiecewiseStepView<T, N>& f)
{
  return MirrorView<T, N>(f);
}

template <std::size_t M, typename T, std::size_t N>
PiecewiseStep<M * N> expand(const PiecewiseStepView<T, N>& f)
{
  std::array<double, M * N> heights;
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < M; j++)
    {
      heights[i * M + j] = f[i];
    }
  }
  return heights;
}

template <typename T, std::size_t N> class NormalizeView : public PiecewiseStepView<NormalizeView<T, N>, N>
{
  const PiecewiseStepView<T, N>& source;
  double mult;

public:
  NormalizeView(const PiecewiseStepView<T, N>& f, double target) : source(f)
  {
    mult = target / source.area();
  }
  double area() const { return source.area() * mult; }
  double operator[](std::size_t i) const { return source[i] * mult; }
};

template <typename T, std::size_t N>
NormalizeView<T, N> normalize(const PiecewiseStepView<T, N>& f, double target = 0.5)
{
  return NormalizeView<T, N>(f, target);
}

template <typename T, typename U, std::size_t N>
std::vector<double> convolute(const PiecewiseStepView<T, N>& f, const PiecewiseStepView<U, N>& g)
{
  alignas(32) std::array<double, N> arr_f, arr_g;
  for (int i = 0; i < N; i++) arr_f[i] = f[i];
  for (int i = 0; i < N; i++) arr_g[i] = g[i];

  constexpr int n = N; // casting from std::size_t
  std::vector<double> result(2 * n - 1);
  for (int k = -n + 1; k < n; k++)
  {
    int i_start = std::max(0, k);
    int i_end   = std::min(n, n + k);
    for (int i = i_start; i < i_end; i++)
    {
      result[k + n - 1] += arr_f[i] * arr_g[i - k];
    }
    result[k + n - 1] /= N;
  }

  return result;
}

} // namespace step_func
