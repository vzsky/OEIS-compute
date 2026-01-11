#include "lib.h"
#include <Treap.h>
#include <array>
#include <cassert>
#include <random>
#include <utils/Utils.h>
#include <vector>

auto score_to_order = [](auto score)
{
  return [score](const auto& a, const auto& b) -> bool
  {
    auto sa = score(a), sb = score(b);
    if (sa != sb) return sa > sb;
    return a > b;
  };
};

template <int Dim> struct Elem
{
  std::array<short, Dim> dim;

  Elem() = default;
  Elem(const std::array<short, Dim>& arr) : dim(arr) {}

  template <int D2> Elem<D2> extend(short x) const
  {
    static_assert(D2 == Dim + 1);
    std::array<short, D2> out{};
    for (int i = 0; i < Dim; i++) out[i] = dim[i];
    out[Dim] = x;
    return Elem<D2>(out);
  }

  bool operator<(const Elem& o) const
  {
    for (int i = 0; i < Dim; i++)
    {
      if (dim[i] != o.dim[i]) return dim[i] < o.dim[i];
    }
    return false;
  }
};

template <int Dim> using Elems = std::vector<Elem<Dim>>;

template <int Dim> bool isSet(const Elem<Dim>& a, const Elem<Dim>& b, const Elem<Dim>& c)
{
  for (int i = 0; i < Dim; i++)
  {
    const bool allEqual = (a.dim[i] == b.dim[i] && b.dim[i] == c.dim[i]);

    const bool allDifferent = (a.dim[i] != b.dim[i] && b.dim[i] != c.dim[i] && a.dim[i] != c.dim[i]);

    if (!allEqual && !allDifferent) return false;
  }
  return true;
}

template <int Dim> bool hasSet(const Elems<Dim>& v)
{
  const size_t n = v.size();
  for (size_t i = 0; i < n; i++)
  {
    for (size_t j = i + 1; j < n; j++)
    {
      for (size_t k = j + 1; k < n; k++)
      {
        if (i == j || j == k || i == k) continue;
        if (isSet(v[i], v[j], v[k])) return true;
      }
    }
  }
  return false;
}

template <int Dim> bool can_add(const Elems<Dim>& parent, const Elem<Dim>& next)
{
  const size_t n = parent.size();
  for (size_t i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      if (isSet(parent[i], parent[j], next)) return false;
    }
  }
  return true;
}

template <int Dim> Elems<Dim> allElems()
{
  Elems<Dim> result;

  Elems<Dim - 1> smaller = allElems<Dim - 1>();
  result.reserve(smaller.size() * 3);

  for (const auto& x : smaller)
  {
    for (short d = 0; d < 3; d++)
    {
      result.push_back(x.template extend<Dim>(d));
    }
  }
  return result;
}

template <> Elems<0> allElems<0>() { return {Elem<0>()}; }

template <int Dim> Elems<Dim> greedyCapSet()
{
  auto all = allElems<Dim>();
  std::shuffle(all.begin(), all.end(), std::mt19937{std::random_device{}()});

  Elems<Dim> out;

  for (auto& x : all)
  {
    out.push_back(x);
    if (hasSet(out)) out.pop_back();
  }

  return out;
}

template <int Dim, typename Heuristic, typename PruneFunc>
Elems<Dim> frontierSearchCapSet(Heuristic h, PruneFunc prune_alg, bool show_smaller_output = false,
                                bool show_progress = false)
{
  using Frontier = Treap<Elems<Dim>, Heuristic>;

  Frontier frontier(h);
  frontier.insert({});

  auto all_elems = allElems<Dim>();
  std::shuffle(all_elems.begin(), all_elems.end(), std::mt19937{std::random_device{}()});

  int smaller_output_cnt = 1;

  // looping from 0 up ensures bit containment ordering.
  // and ensure we get all the smaller N solutions
  for (Elem<Dim>& next : all_elems)
  {

    std::vector to_add = frontier.gather([&next](const auto& parent)
    {
      if (can_add(parent, next))
      {
        Elems<Dim> child = parent;
        child.push_back(next);
        return std::optional<Elems<Dim>>{child};
      }
      return std::optional<Elems<Dim>>{};
    }, 16, 100);

    for (auto& s : to_add)
    {
      if (s.second.has_value()) frontier.insert(std::move(*s.second));
    }

    prune_alg(frontier);
  }
  return frontier.max([](const auto& x) { return x.size(); }, Elems<Dim>());
}

int main()
{

  auto heuristic = [](const auto& v) -> int64_t { return v.size(); };

  auto prune = [](auto& frontier)
  {
    constexpr int SIZE_KEEP  = 6000;
    constexpr int SIZE_LIMIT = 60000;
    if (frontier.size() < SIZE_LIMIT) return;
    frontier.restrict(SIZE_KEEP);
  };

  /*
    { auto v = allElems<1>();
      auto setFree = frontierSearchCapSet<1>(score_to_order(heuristic), prune);
      assert(!hasSet(setFree));
      std::cout << setFree.size() << std::endl; }
    { auto v = allElems<2>();
      auto setFree = frontierSearchCapSet<2>(score_to_order(heuristic), prune);
      assert(!hasSet(setFree));
      std::cout << setFree.size() << std::endl; }
    { auto v = allElems<3>();
      auto setFree = frontierSearchCapSet<3>(score_to_order(heuristic), prune);
      assert(!hasSet(setFree));
      std::cout << setFree.size() << std::endl; }
    { auto v = allElems<4>();
      auto setFree = frontierSearchCapSet<4>(score_to_order(heuristic), prune);
      assert(!hasSet(setFree));
      std::cout << setFree.size() << std::endl; }
  */

  {
    constexpr int Dim = 5;
    auto v            = allElems<Dim>();
    auto setFree      = frontierSearchCapSet<Dim>(score_to_order(heuristic), prune);
    assert(!hasSet(setFree));
    std::cout << setFree.size() << std::endl;
  }
}
