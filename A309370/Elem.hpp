#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

template <int N> struct Elem
{
  uint64_t mUnd;

  Elem() : mUnd{0} {}
  Elem(uint64_t bin) : mUnd{bin} {}

  operator int() const { return mUnd; }
  uint64_t operator[](const int idx) const { return (mUnd >> idx) & 1; }

  bool operator==(const Elem<N>& o) const { return mUnd == o.mUnd; }
  bool operator<(const Elem<N>& o) const { return mUnd < o.mUnd; }

  void print(int n = N, std::ostream& os = std::cout) const
  {
    for (int i = 0; i < n; i++) os << ((mUnd >> i) & 1);
  }

  friend std::ostream& operator<<(std::ostream& os, const Elem& e)
  {
    e.print(N, os);
    return os;
  }
};

template <int N> struct Elems
{
  std::vector<Elem<N>> mVec;
  size_t mCreated;
  size_t mLastUsed;

  Elems() : mVec{}, mCreated{0}, mLastUsed{0} {}

  size_t size() const { return mVec.size(); }
  void push_back(const Elem<N>& x) { mVec.push_back(x); }
  void push_back(Elem<N>&& x) { mVec.push_back(std::move(x)); }
  void pop_back() { mVec.pop_back(); }
  void resize(size_t size) { mVec.resize(size); }

  auto begin() { return mVec.begin(); }
  auto end() { return mVec.end(); }
  auto begin() const { return mVec.begin(); }
  auto end() const { return mVec.end(); }

  auto back() const { return mVec.back(); }

  Elem<N>& operator[](size_t i) { return mVec[i]; }
  const Elem<N>& operator[](size_t i) const { return mVec[i]; }

  void print(int n = N, std::ostream& os = std::cout) const
  {
    os << "size: " << size() << "\n";
    for (const auto& x : *this)
    {
      x.print(n, os);
      os << '\n';
    }
  }

  friend std::ostream& operator<<(std::ostream& os, const Elems<N>& v)
  {
    v.print(N, os);
    return os;
  }

  friend bool operator>(const Elems& a, const Elems& b) { return a.mVec > b.mVec; }

  bool contains(const Elem<N>& x) const
  {
    for (const auto& e : mVec)
      if (e == x) return true;
    return false;
  }
};

template <int N> Elems<N> parse_from_file(const std::string& filename)
{

  std::ifstream fin(filename);
  if (!fin)
  {
    throw std::runtime_error("Cannot open file: " + filename);
  }
  Elems<N> result;
  std::string line;

  while (std::getline(fin, line))
  {
    if (line.empty()) continue;
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r' || line.back() == ' '))
      line.pop_back();
    if (line.size() != N)
    {
      std::cerr << "Skipping line: incorrect length = " << line.size() << ", expected " << N << "\n";
      continue;
    }

    uint64_t bits = 0;
    for (int i = 0; i < N; i++)
    {
      char c = line[i];
      if (c == '1')
        bits |= (uint64_t(1) << i);
      else if (c != '0')
      {
        std::cerr << "Skipping invalid line: contains non-binary char\n";
        bits = 0;
        break;
      }
    }
    result.push_back(Elem<N>(bits));
  }
  return result;
}
