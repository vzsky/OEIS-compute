#include <math/Basic.hpp>
#include <utils/Parallel.hpp>
#include <utils/Prime.hpp>
#include <utils/Utils.hpp>
#include <vector>

namespace {

struct NoOp
{
};

template <typename OnStep = NoOp> uint64_t concat_mod(uint64_t n, uint64_t mod, OnStep&& on_step = {})
{
  if (mod == 1) return 0;

  uint64_t pow2[65] = {};
  pow2[1]           = 2 % mod;
  for (int b = 2; b < 65; b++) pow2[b] = pow2[b - 1] * 2 % mod;

  uint64_t result = 0, shift_mod = 1;
  for (uint64_t bits = 1; bits <= 64; bits++)
  {
    uint64_t lo = 1ull << (bits - 1);
    if (lo > n) break;
    uint64_t hi = (bits < 64) ? std::min(n, (1ull << bits) - 1) : n;
    uint64_t pw = pow2[bits];

    for (uint64_t k = lo; k <= hi; k++)
    {
      uint64_t rev = (__builtin_bitreverse64(k) >> (64 - bits)) % mod;
      result       = (result + rev * shift_mod) % mod;
      shift_mod    = shift_mod * pw % mod;
      if constexpr (!std::is_same_v<OnStep, NoOp>) on_step(k, result);
    }
  }
  return result;
}

} // namespace

int main()
{
  utils::ScopeTimer _t{"main"};
  namespace prl = utils::parallel;

  // ran [10M, 50M] (28h run), doesn't find any
  constexpr prl::MonitorConfig monConf{.intervalSeconds = 3};
  constexpr size_t N          = 50'000'000;
  constexpr size_t minN       = 10'000'000;
  constexpr size_t sieveLimit = 7072; // ceil(sqrt(5e7))

  Log(LL::Info, "N=$  minN=$  sieveLimit=$"_f, N, minN, sieveLimit);

  Prime<N> allPrimes;
  std::vector<uint8_t> valid(N + 1, 1);

  {
    utils::ScopeTimer _t{"sieve"};
    std::vector<uint64_t> sieveMods;
    for (auto p : allPrimes.all_primes())
    {
      if (p > sieveLimit) break;
      for (uint64_t pk = p; pk <= sieveLimit; pk *= p) sieveMods.push_back(pk);
    }
    Log(LL::Info, "Sieve moduli = $"_f, sieveMods.size());

    // benign race: threads only ever write 0
    prl::foreach<monConf>(sieveMods, [&](uint64_t m)
    {
      concat_mod(N, m, [&](uint64_t k, uint64_t result)
      {
        if (k % m == 0 && result != 0) valid[k] = 0;
      });
    });
  }

  std::vector<int> candidates;
  {
    utils::ScopeTimer _t{"filter"};
    for (size_t n = minN; n <= N; n++)
    {
      if (!valid[n]) continue;
      candidates.push_back(static_cast<int>(n));
    }
    Log(LL::Info, "candidates:", candidates.size());
  }

  {
    utils::ScopeTimer _t{"compute"};
    size_t T = std::thread::hardware_concurrency();
    std::vector<int> balanced(candidates.size());
    size_t idx = 0;
    for (size_t t = 0; t < T; t++)
      for (size_t i = t; i < candidates.size(); i += T) balanced[idx++] = candidates[i];

    prl::foreach<monConf>(balanced, [](int n)
    {
      if (concat_mod(n, n) == 0) Log(LL::Info, n);
    });
  }
}
