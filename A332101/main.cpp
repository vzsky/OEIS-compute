#include "Logging.hpp"
#include <math/Basic.hpp>
#include <utils/BigInt.hpp>
#include <utils/Utils.hpp>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>

// A332101: a(n) = least m such that m^n <= Sum_{k=1}^{m-1} k^n.
namespace {

// TODO maya::Tagged

// with double precision, m^n <= sum (k^n)
bool approx_holds(int n, std::size_t m)
{
  if (m < 2) return false;

  const double dn   = n;
  const double logB = std::log(static_cast<double>(m - 1));

  double inner = 0.0; // Sum_{k=1}^{m-1} (k/(m-1))^n, each term in (0, 1]
  for (std::size_t k = 1; k < m; ++k) inner += std::exp(n * (std::log(static_cast<double>(k)) - logB));
  const double logSum = dn * logB + std::log(inner); // ~ log(Sum_{k<m} k^n)

  const double logTop = dn * std::log(static_cast<double>(m));
  return logTop <= logSum;
}

BigInt power_sum_below(int n, std::size_t m)
{
  BigInt sum = 0;
  for (std::size_t k = 1; k < m; ++k) sum += math::pow(BigInt(k), static_cast<uint64_t>(n));
  return sum;
}

bool exact_holds(int n, std::size_t m)
{
  return math::pow(BigInt(m), static_cast<uint64_t>(n)) <= power_sum_below(n, m);
}

std::size_t solve(int n)
{
  logging::Scope _l = logging::Env{}.logger(loggers::progress);

  std::size_t hi = 2;
  while (!approx_holds(n, hi)) hi *= 2;

  std::size_t lo = hi / 2;
  while (lo + 1 < hi)
  {
    std::size_t mid                  = lo + (hi - lo) / 2;
    (approx_holds(n, mid) ? hi : lo) = mid;
    Log(LL::Debug, "$ <= a($) <= $"_f, lo, n, hi);
  }
  std::size_t m = hi;

  return m;
}

// Conjecture: a(n) = round(n / ln 2) + 2.
std::size_t solve_conjecture(int n) { return static_cast<std::size_t>(std::llround(n / std::log(2.0))) + 2; }

void check(int n, std::size_t m)
{
  logging::Scope _l = logging::Env{}.logger(loggers::progress);

  const uint64_t e = static_cast<uint64_t>(n);

  BigInt S   = power_sum_below(n, m);
  BigInt top = math::pow(BigInt(m), e);

  int adjustments = 0;
  while (top > S)
  {
    S += top;
    ++m;
    top = math::pow(BigInt(m), e);
    ++adjustments;
  }
  while (m > 2)
  {
    BigInt below = math::pow(BigInt(m - 1), e);
    BigInt sumLo = S - below;
    if (below > sumLo) break;
    S   = sumLo;
    top = below;
    --m;
    ++adjustments;
  }

  const BigInt below = math::pow(BigInt(m - 1), e);
  assert(top <= S && "verification failed: m^n > Sum_{k<m} k^n");
  assert(below > S - below && "verification failed: m-1 also holds, so m is not least");

  // TODO log progress
  Log(LL::Info, "a($) = $ [$ adjustments]"_f, n, m, adjustments);
  if (adjustments)
  {
    logging::Scope _l = logging::Env{}.logger(loggers::normal);
    Log(LL::Info, "Conjecture is false");
  }
}

} // namespace

int main()
{
  utils::ScopeTimer _t{"main"};
  constexpr int N = 1000;

  // TODO: can just build one sum rather than N
  for (int n = 1; n <= N; ++n)
  {
    check(n, solve_conjecture(n));
  }
}
