#pragma once

#include <cassert>
#include <map>
#include <vector>

template <int N> class Prime
{
public:
  Prime() : mLowestPrime(N)
  {
    sieve();
  }

private:
  void sieve();

public:
  std::vector<int> factors(int n) const;
  std::map<int, int> factors_freq(int n) const;
  std::vector<int> all_primes() const;

private:
  std::vector<int> mLowestPrime{N};
};

template <int N> void Prime<N>::sieve()
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

template <int N> std::vector<int> Prime<N>::all_primes() const
{
  std::vector<int> primes;
  for (int i = 2; i <= N; i++)
  {
    if (mLowestPrime[i] == i)
      primes.push_back(i);
  }
  return primes;
}

template <int N> std::vector<int> Prime<N>::factors(int n) const
{
  assert(2 <= n && n <= N);
  std::vector<int> f;
  while (n > 1)
  {
    f.push_back(mLowestPrime[n]);
    n /= mLowestPrime[n];
  }
  return f;
}

template <int N> std::map<int, int> Prime<N>::factors_freq(int n) const
{
  std::map<int, int> freq;
  for (int p : factors(n))
    freq[p]++;
  return freq;
}
