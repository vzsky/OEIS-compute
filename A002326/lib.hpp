#pragma once

#include <math/Basic.hpp>
#include <utils/Prime.hpp>

template <size_t N> class A002326
{
private:
  // assume total = (p1^a1 ... pn^an)
  // returns minimum k such that n = (p1^a1 ... p^k ... pn^an) still make 2^n =
  // 1 p^k should divides total and 2^total = 1
  uint64_t min_p_needed(uint64_t total, uint64_t p, uint64_t k, uint64_t mod)
  {
    uint64_t l = 0, r = k;
    while (l < r)
    {
      uint64_t mid = (l + r) / 2;
      uint64_t now = total / math::pow(p, k - mid);
      if (math::pow(2ull, now, mod) == 1)
        r = mid;
      else
        l = mid + 1;
    }
    return l;
  }

  int order_of_2_mod_p(int prime)
  {
    uint64_t n     = prime;
    uint64_t phi_n = prime - 1;
    auto primes    = factorizer.factors_freq(phi_n);
    uint64_t ans   = 1;
    for (auto& [p, k] : primes)
    {
      uint64_t x = min_p_needed(phi_n, p, k, n);
      ans *= math::pow(p, x);
    }
    return ans;
  }

  uint64_t order_of_2_mod_prime_power(uint64_t p, uint64_t power)
  {
    if (power == 1) return order_of_2_mod_p(p);
    uint64_t cur = order_of_2_mod_p(p);
    uint64_t mod = math::pow(p, power);
    uint64_t l   = 0;
    uint64_t r   = power - 1;
    while (l < r)
    {
      int mid = (l + r) / 2;
      if (math::pow(2ull, cur * math::pow(p, mid), mod) == 1)
        r = mid;
      else
        l = mid + 1;
    }
    return cur * math::pow(p, l);
  }

public:
  uint64_t get_answer(uint64_t n)
  {
    if (n == 1) return 1;
    n -= 1;
    auto factors = factorizer.factors_freq(2 * n + 1);
    uint64_t ans = 1;
    for (auto& [p, k] : factors)
    {
      auto res = order_of_2_mod_prime_power(p, k);
      ans      = math::lcm(ans, res);
    }
    return ans;
  }

  std::vector<uint64_t> get_answers_until(uint64_t until)
  {
    std::vector<uint64_t> all_answers(until - 1);
    for (size_t ind = 2; ind <= until; ind++)
    {
      uint64_t n   = 2 * ind - 1;
      uint64_t ans = 1;
      auto factors = factorizer.factors_freq(n);
      if (factors.size() == 1)
      {
        uint64_t p = factors.begin()->first;
        uint64_t k = factors.begin()->second;
        if (k == 1)
          ans = order_of_2_mod_p(p);
        else
        {
          int prev_ind = math::pow(p, k - 1) / 2 - 1;
          ans          = all_answers[prev_ind];
          if (math::pow(2ull, all_answers[prev_ind], n) != 1) ans *= p;
        }
      }
      else
      {
        for (auto& [p, k] : factors)
        {
          ans = math::lcm(ans, all_answers[math::pow(p, k) / 2 - 1]);
        }
      }
      all_answers[ind - 2] = ans;
    }
    return all_answers;
  }

private:
  PrimeSieve<2 * N + 1> factorizer;
};
