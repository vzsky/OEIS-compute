#include <gtest/gtest.h>

#include <atomic>
#include <numeric>
#include <vector>

#include <utils/Parallel.hpp>

namespace p = utils::parallel;

// ── all_of ────────────────────────────────────────────────────────────────────

TEST(ParallelAllOf, EmptyRange)
{
  std::vector<int> v;
  EXPECT_TRUE(p::all_of(v, [](int) { return false; }));
}

TEST(ParallelAllOf, SingleElement)
{
  std::vector<int> v{7};
  EXPECT_TRUE(p::all_of(v, [](int x) { return x == 7; }));
  EXPECT_FALSE(p::all_of(v, [](int x) { return x == 0; }));
}

TEST(ParallelAllOf, AllTrue)
{
  std::vector<int> v(1000);
  std::iota(v.begin(), v.end(), 1);
  EXPECT_TRUE(p::all_of(v, [](int x) { return x > 0; }));
}

TEST(ParallelAllOf, AllFalse)
{
  std::vector<int> v(1000);
  std::iota(v.begin(), v.end(), 1);
  EXPECT_FALSE(p::all_of(v, [](int x) { return x < 0; }));
}

TEST(ParallelAllOf, SomeFalse)
{
  std::vector<int> v(1000);
  std::iota(v.begin(), v.end(), 0);
  EXPECT_FALSE(p::all_of(v, [](int x) { return x % 2 == 0; }));
}

TEST(ParallelAllOf, SingleThread)
{
  std::vector<int> v(100);
  std::iota(v.begin(), v.end(), 1);
  EXPECT_TRUE(p::all_of(v, [](int x) { return x > 0; }, 1));
  EXPECT_FALSE(p::all_of(v, [](int x) { return x > 50; }, 1));
}

// ── foreach ───────────────────────────────────────────────────────────────────

TEST(ParallelForeach, EmptyRange)
{
  std::vector<int> v;
  std::atomic<int> count{0};
  p::foreach (v, [&](int) { count++; });
  EXPECT_EQ(count.load(), 0);
}

TEST(ParallelForeach, VisitsAllElements)
{
  const size_t n = 1000;
  std::vector<int> v(n);
  std::iota(v.begin(), v.end(), 0);

  std::vector<std::atomic<int>> seen(n);
  for (auto& a : seen) a.store(0);

  p::foreach (v, [&](int x) { seen[x].fetch_add(1); });

  for (size_t i = 0; i < n; ++i) EXPECT_EQ(seen[i].load(), 1);
}

TEST(ParallelForeach, SingleThread)
{
  std::vector<int> v(100);
  std::iota(v.begin(), v.end(), 0);
  std::atomic<int> sum{0};
  p::foreach (v, [&](int x) { sum.fetch_add(x); }, 1);
  EXPECT_EQ(sum.load(), 99 * 100 / 2);
}

TEST(ParallelForeach, IteratorOverload)
{
  std::vector<int> v = {1, 2, 3, 4, 5};
  std::atomic<int> sum{0};
  p::foreach (v.begin(), v.end(), [&](int x) { sum.fetch_add(x); });
  EXPECT_EQ(sum.load(), 15);
}

// ── filter ────────────────────────────────────────────────────────────────────

TEST(ParallelFilter, EmptyRange)
{
  std::vector<int> v;
  auto result = p::filter(v, [](int) { return true; });
  EXPECT_TRUE(result.empty());
}

TEST(ParallelFilter, NonePass)
{
  std::vector<int> v = {1, 3, 5, 7};
  auto result        = p::filter(v, [](int x) { return x % 2 == 0; });
  EXPECT_TRUE(result.empty());
}

TEST(ParallelFilter, AllPass)
{
  std::vector<int> v = {2, 4, 6, 8};
  auto result        = p::filter(v, [](int x) { return x % 2 == 0; });
  EXPECT_EQ(result.size(), v.size());
}

TEST(ParallelFilter, SomePass)
{
  const size_t n = 1000;
  std::vector<int> v(n);
  std::iota(v.begin(), v.end(), 0);

  auto result = p::filter(v, [](int x) { return x % 2 == 0; });

  ASSERT_EQ(result.size(), n / 2);
  for (int x : result) EXPECT_EQ(x % 2, 0);
}

TEST(ParallelFilter, CorrectValues)
{
  std::vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6};
  auto result        = p::filter(v, [](int x) { return x > 5; });

  std::sort(result.begin(), result.end());
  EXPECT_EQ(result, (std::vector<int>{6, 7, 8, 9}));
}

TEST(ParallelFilter, SingleThread)
{
  std::vector<int> v(100);
  std::iota(v.begin(), v.end(), 1);
  auto result = p::filter(v, [](int x) { return x % 3 == 0; }, 1);
  for (int x : result) EXPECT_EQ(x % 3, 0);
  EXPECT_EQ(result.size(), 33u);
}
