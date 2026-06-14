#include <math/Basic.hpp>
#include <utils/BigInt.hpp>
#include <utils/Logging.hpp>
#include <utils/Utils.hpp>

#include <array>
#include <cmath>
#include <cstddef>

namespace {

// Logger
constexpr uint32_t LogThrottlePeriod = 50;
void progress_write(std::string_view msg)
{
  static uint32_t count = 0;
  if (++count % LogThrottlePeriod == 0) std::cout << '\r' << msg << "\033[K" << std::flush;
}
void progress_done() { std::cout << '\n' << std::flush; }
const logging::Logger throttled_progress{&progress_write, &progress_done};

} // namespace

// A332101: a(n) = least m such that m^n <= Sum_{k=1}^{m-1} k^n.
namespace {

using Exp = maya::Tagged<struct exp_tag, uint64_t>;

// Conjecture: a(n) = round(n / ln 2) + 2.
uint64_t solve_conjecture(Exp n) { return static_cast<uint64_t>(std::llround(n / std::log(2.0))) + 2; }

static constexpr std::size_t active_limit(uint64_t n) { return static_cast<std::size_t>(1.45 * n) + 10; }

template <uint64_t MaxN> class Solver
{
public:
  // Initializes at exponent 1 and silently advances to start.
  explicit Solver(uint64_t start) : mExp{1}, mActiveLimit{active_limit(1)}, mKpow{}
  {
    for (std::size_t k = 1; k <= mActiveLimit; ++k) mKpow[k] = k;
    for (uint64_t n = 2; n <= start; ++n) advance();
  }

  void next_exponent() { advance(); }

  uint64_t compute()
  {
    BigInt sum    = 0;
    std::size_t m = 1;
    while (mKpow[m] > sum)
    {
      sum += mKpow[m];
      ++m;
    }
    return m;
  }

private:
  void advance()
  {
    for (std::size_t k = 2; k <= mActiveLimit; ++k) mKpow[k] *= k;
    ++mExp;
    const std::size_t newlimit = active_limit(mExp);
    for (std::size_t k = mActiveLimit + 1; k <= newlimit; ++k) mKpow[k] = math::pow(BigInt(k), mExp);
    mActiveLimit = newlimit;
  }

  Exp mExp;
  std::size_t mActiveLimit;
  std::array<BigInt, MaxN + 1> mKpow;
};

} // namespace

int main()
{
  utils::ScopeTimer _t{"main"};
  logging::Scope _l = logging::Env{}.logger(throttled_progress);

  constexpr uint64_t Start = 1;
  constexpr uint64_t Stop  = 50000;
  // 1 to 50k complete, conjecture confirmed in 8650 seconds

  Solver<active_limit(Stop)> solver{Start};

  for (Exp n{Start}; n <= Stop; n = n + 1)
  {
    if (n > Start) solver.next_exponent();
    const uint64_t m     = solver.compute();
    const uint64_t guess = solve_conjecture(n);
    Log(LL::Info, "a($) = $"_f, n, m);
    if (m != guess)
    {
      logging::Scope _l = logging::Env{}.logger(loggers::normal);
      Log(LL::Warn, "conjecture wrong: expected $"_f, guess);
    }
  }
}
