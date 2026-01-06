#pragma once

#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

template <size_t N> class Prime
{
public:
  Prime();

  bool is_prime(size_t n) const;
  std::vector<size_t> factors(size_t n) const;
  std::vector<size_t> fast_factors(size_t n) const;
  std::map<size_t, size_t> factors_freq(size_t n) const;
  std::vector<std::pair<size_t, size_t>> vector_factors_freq(size_t n) const;
  std::vector<size_t> all_primes() const;

private:
  std::vector<size_t> mLowestPrimeDiv =
      std::vector<size_t>(N + 1); // lowest prime divisor of i
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

template <size_t N> std::vector<size_t> Prime<N>::all_primes() const
{
  return mAllPrimes;
}

template <size_t N> std::vector<size_t> Prime<N>::factors(size_t n) const
{
  if (2 <= n && n <= N) return fast_factors(n);

  std::vector<size_t> result;

  auto checkDivisor = [&](size_t& n, size_t d)
  {
    if (n == 1) return;

    while (n % d == 0)
    {
      result.push_back(d);
      n /= d;
    }
  };

  for (auto p : mAllPrimes)
  {
    checkDivisor(n, p);
  }

  size_t next = mAllPrimes.back() + 1;
  while (next * next <= n)
  {
    checkDivisor(n, next);
    next++;
  }

  if (n != 1) result.push_back(n);

  return result;
}

template <size_t N> std::vector<size_t> Prime<N>::fast_factors(size_t n) const
{
  if (!(1 <= n && n <= N))
    throw std::invalid_argument("prime factoring: n out of range");

  if (n == 1) return std::vector<size_t>{};

  std::vector<size_t> f;
  while (n > 1)
  {
    f.push_back(mLowestPrimeDiv[n]);
    n /= mLowestPrimeDiv[n];
  }
  return f;
}

template <size_t N>
std::map<size_t, size_t> Prime<N>::factors_freq(size_t n) const
{
  std::map<size_t, size_t> freq;
  for (size_t p : factors(n)) freq[p]++;
  return freq;
}

template <size_t N>
std::vector<std::pair<size_t, size_t>>
Prime<N>::vector_factors_freq(size_t n) const
{
  std::vector<std::pair<size_t, size_t>> freq;
  for (size_t p : factors(n))
  {
    if (freq.empty() || freq.back().first != p)
      freq.emplace_back(p, 1);
    else
      freq.back().second++;
  }
  return freq;
}
