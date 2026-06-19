#include <math/Basic.hpp>
#include <utils/BigInt.hpp>
#include <utils/Logging.hpp>
#include <utils/Utils.hpp>

#include <cstddef>
#include <vector>

// A332101: a(n) = least m such that m^n <= Sum_{k=1}^{m-1} k^n.
// Seems like we can do better using bigfloat?
namespace {

using Exp = maya::Tagged<struct exp_tag, uint64_t>;

// Conjecture: a(n) = round(n / ln 2) + 2.
uint64_t solve_conjecture(Exp n)
{
  static const BigInt P("14426950408889634073599246810018921374266459541529859341354"); // 1/ln2
  static const BigInt Q = math::pow(BigInt(10), 58);
  return ((BigInt(n.get()) * P + Q / 2) / Q).to_uint64() + 2;
}

class Solver
{
public:
  explicit Solver(uint64_t start) : mExp{start}, mKpow{0, 1} {}

  [[nodiscard]] uint64_t compute_next_exponent()
  {
    ++mExp;
    for (std::size_t k = 2; k < mKpow.size(); ++k) mKpow[k] *= k;

    BigInt sum    = 0;
    std::size_t m = 1;
    while (true)
    {
      if (m >= mKpow.size()) mKpow.push_back(math::pow(BigInt(m), mExp));
      if (mKpow[m] <= sum) return m;
      sum += mKpow[m];
      ++m;
    }
  }

private:
  logging::Scope _l = logging::Env{}.module("solver");
  Exp mExp;
  std::vector<BigInt> mKpow;
};

class ExactConjCheck
{
public:
  // check if m^n <= sum k^n
  [[nodiscard]] static bool check(uint64_t m, Exp n)
  {
    logging::Scope _l = logging::Env{}.module("exactCheck").logger(loggers::progress);

    BigInt remaining = math::pow(BigInt(m), n);

    for (uint64_t k = m - 1; k >= 1; --k)
    {
      BigInt term  = math::pow(BigInt(k), n);
      BigInt under = term * k;

      // remaining terms 1^n+...+k^n < k^{n+1} = term*k; if sum still exceeds that, can't flip
      if (remaining > under) return false;

      remaining -= term;

      uint64_t remaining_bits = remaining.magnitude();
      uint64_t under_bits     = under.magnitude();
      Log(LL::Info, "iter=$ k=$ remaining=$'b underbound=$'b diff=$'b"_f, m - k, k, remaining_bits,
          under_bits, under_bits - remaining_bits);
      Log(LL::Debug, "k=$ remaining=$"_f, k, remaining);

      if (remaining <= 0) return true;
    }

    return false;
  }
};

} // namespace

void check_conjecture_loop()
{
  logging::Scope _l = logging::Env{}.logger(loggers::progress);

  constexpr uint64_t Start = 52000;
  constexpr uint64_t Stop  = 60000;
  // 1 to 50k complete, conjecture confirmed in 8650 seconds
  //
  Solver solver{Start - 1};

  for (Exp n{Start}; n <= Stop; n = n + 1)
  {
    const uint64_t m     = solver.compute_next_exponent();
    const uint64_t guess = solve_conjecture(n);
    Log(LL::Info, "a($) = $"_f, n, m);
    if (m != guess)
    {
      logging::Scope _l = logging::Env{}.logger(loggers::normal);
      Log(LL::Warn, "conjecture wrong: expected $"_f, guess);
    }
  }
}

void check_close_candidates()
{
  logging::Scope _l = logging::Env{}.logger(loggers::normal);
  // candidate n are the one where n/ln(2) near 0.5

  // checked and conjecture holds: 2639344, 10975455, 24590254
  // for (uint64_t n : {418079, 2639344, 10975455, 24590254})
  for (uint64_t n : {418079ull, 248541884ull, 124492254286ull, 574033809103ull, 777451915729368ull})
  {
    utils::ScopeTimer _t{"candidateCheck"};

    const uint64_t guess = solve_conjecture(Exp{n});
    Log(LL::Info, "checking n=$, conj=$, check_at=$"_f, n, guess, guess - 1);
    bool conj_break = ExactConjCheck::check(guess - 1, Exp{n});
    Log(LL::Info, "n=$  conjecture=$ conjecture_break=$"_f, n, guess, conj_break);
    if (conj_break) Log(LL::Info, "Conjecture is false at n=$"_f, n);
    bool conj_holds = ExactConjCheck::check(guess, Exp{n});
    if (conj_holds) Log(LL::Info, "Conjecture holds at n=$"_f, n);
  }
}

int main()
{
  utils::ScopeTimer _t{"main"};
  logging::Scope _l = logging::Env{}.logger(loggers::normal);

  // check_conjecture_loop();

  check_close_candidates();
}
