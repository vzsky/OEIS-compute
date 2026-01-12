#pragma once

#include <map>
#include <stdexcept>
#include <vector>

template <size_t N> class Prime
{
public:
  Prime();

  bool is_prime(size_t n) const;
  std::vector<size_t> factors(size_t n) const;
  std::map<size_t, size_t> factors_freq(size_t n) const;
  std::vector<std::pair<size_t, size_t>> vector_factors_freq(size_t n) const;
  const std::vector<size_t>& all_primes() const;

private:
  template <typename EmitCB> void emit_factors(size_t, EmitCB) const;

  std::vector<size_t> mLowestPrimeDiv = std::vector<size_t>(N + 1); // lowest prime divisor of i
  std::vector<size_t> mAllPrimes;
};

template <size_t N> Prime<N>::Prime()
{
  for (size_t i = 0; i <= N; ++i) mLowestPrimeDiv[i] = i;

  for (size_t i = 2; i * i <= N; ++i)
  {
    if (mLowestPrimeDiv[i] == i)
    {
      for (size_t j = i * i; j <= N; j += i)
      {
        if (mLowestPrimeDiv[j] == j) mLowestPrimeDiv[j] = i;
      }
    }
  }

  for (size_t i = 2; i <= N; i++)
  {
    if (mLowestPrimeDiv[i] == i) mAllPrimes.push_back(i);
  }
}

template <size_t N> bool Prime<N>::is_prime(size_t n) const
{
  if (n == 0) throw std::invalid_argument("is prime: n out of range");
  if (n == 1) return false;
  if (n <= N) return mLowestPrimeDiv[n] == n;

  for (auto p : mAllPrimes)
    if (n % p == 0) return false;

  size_t next = mAllPrimes.back() + 1;
  while (next * next <= n)
  {
    if (n % next == 0) return false;
    next++;
  }

  return true;
}

template <size_t N> const std::vector<size_t>& Prime<N>::all_primes() const { return mAllPrimes; }

template <size_t N> template <typename EmitCB> void Prime<N>::emit_factors(size_t n, EmitCB callback) const
{
  auto fastFactor = [&](size_t& n)
  {
    while (n != 1)
    {
      callback(mLowestPrimeDiv[n]);
      n /= mLowestPrimeDiv[n];
    }
  };

  if (n <= N) return fastFactor(n);

  auto checkDivisor = [&](size_t& n, size_t d)
  {
    if (n == 1) return;
    while (n != 1 && n % d == 0)
    {
      callback(d);
      n /= d;
      if (n <= N) fastFactor(n);
    }
  };

  for (auto p : mAllPrimes) checkDivisor(n, p);

  size_t next = mAllPrimes.back() + 1;
  while (next * next <= n)
  {
    checkDivisor(n, next);
    next++;
  }

  if (n != 1) callback(n);
}

template <size_t N> std::vector<size_t> Prime<N>::factors(size_t n) const
{
  std::vector<size_t> result;
  emit_factors(n, [&](size_t p) { result.push_back(p); });
  return result;
}

template <size_t N> std::map<size_t, size_t> Prime<N>::factors_freq(size_t n) const
{
  std::map<size_t, size_t> freq;
  emit_factors(n, [&](size_t p) { ++freq[p]; });
  return freq;
}

template <size_t N> std::vector<std::pair<size_t, size_t>> Prime<N>::vector_factors_freq(size_t n) const
{
  std::vector<std::pair<size_t, size_t>> freq;
  emit_factors(n, [&](size_t p)
  {
    if (freq.empty() || freq.back().first != p)
      freq.emplace_back(p, 1);
    else
      ++freq.back().second;
  });
  return freq;
}
