#pragma once

#include <map>
#include <stdexcept>
#include <vector>

template <size_t N> class Prime
{
public:
  Prime() : mLowestPrime(N)
  {
    sieve();
  }

private:
  void sieve();

public:
  bool is_prime(int n) const;
  std::vector<int> factors(int n) const;
  std::map<int, int> factors_freq(int n) const;
  std::vector<std::pair<int, int>> vector_factors_freq(int n) const;
  std::vector<int> all_primes() const;

private:
  std::vector<int> mLowestPrime{N}; // lowest prime divisor of i
};

template <size_t N> void Prime<N>::sieve()
{
  for (int i = 0; i <= N; ++i)
    mLowestPrime[i] = i;
  for (int i = 2; i * i <= N; ++i)
  {
    if (mLowestPrime[i] == i)
    {
      for (int j = i * i; j <= N; j += i)
      {
        if (mLowestPrime[j] == j)
          mLowestPrime[j] = i;
      }
    }
  }
}

template <size_t N> bool Prime<N>::is_prime(int n) const
{
  return mLowestPrime[n] == n;
}

template <size_t N> std::vector<int> Prime<N>::all_primes() const
{
  std::vector<int> primes;
  for (int i = 2; i <= N; i++)
  {
    if (mLowestPrime[i] == i)
      primes.push_back(i);
  }
  return primes;
}

template <size_t N> std::vector<int> Prime<N>::factors(int n) const
{
  if (!(2 <= n && n <= N))
    throw std::invalid_argument("n out of range");

  std::vector<int> f;
  while (n > 1)
  {
    f.push_back(mLowestPrime[n]);
    n /= mLowestPrime[n];
  }
  return f;
}

template <size_t N> std::map<int, int> Prime<N>::factors_freq(int n) const
{
  std::map<int, int> freq;
  for (int p : factors(n))
    freq[p]++;
  return freq;
}

template <size_t N> std::vector<std::pair<int, int>> Prime<N>::vector_factors_freq(int n) const
{
  std::vector<std::pair<int, int>> freq;
  for (int p : factors(n))
  {
    if (freq.empty() || freq.back().first != p)
      freq.emplace_back(p, 1);
    else
      freq.back().second++;
  }
  return freq;
}
