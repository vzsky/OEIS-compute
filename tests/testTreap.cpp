#include <algorithm>
#include <gtest/gtest.h>
#include <utils/Treap.h>
#include <vector>

TEST(TreapTest, BasicInsertAndAccess)
{
  Treap<int, std::less<int>> t(std::less<int>{});
  t.insert(5);
  t.insert(3);
  t.insert(7);

  ASSERT_EQ(t.size(), 3);
  ASSERT_EQ(t[0], 3);
  ASSERT_EQ(t[1], 5);
  ASSERT_EQ(t[2], 7);
}

TEST(TreapTest, Duplicates)
{
  Treap<int, std::less<int>> t(std::less<int>{});
  t.insert(1);
  t.insert(1);
  t.insert(1);

  ASSERT_EQ(t.size(), 3);
  ASSERT_EQ(t[0], 1);
  ASSERT_EQ(t[1], 1);
  ASSERT_EQ(t[2], 1);
}

TEST(TreapTest, RestrictKeepsSmallest)
{
  Treap<int, std::less<int>> t(std::less<int>{});
  for (int i = 1; i <= 10; ++i) t.insert(std::move(i));

  t.restrict(5); // keep first 5 smallest
  ASSERT_EQ(t.size(), 5);

  std::vector<int> expected{1, 2, 3, 4, 5};
  for (int i = 0; i < t.size(); ++i) ASSERT_EQ(t[i], expected[i]);
}

TEST(TreapTest, ContainsWorks)
{
  Treap<int, std::less<int>> t(std::less<int>{});
  t.insert(10);
  t.insert(20);
  t.insert(30);

  ASSERT_TRUE(t.contains(10));
  ASSERT_FALSE(t.contains(15));
  ASSERT_TRUE(t.contains(20));
  ASSERT_FALSE(t.contains(5));
}

TEST(TreapTest, GatherIterateAndMap)
{
  Treap<int, std::less<int>> t(std::less<int>{});

  std::vector<int> expected{};
  for (int i = 1; i <= 5; ++i)
  {
    expected.push_back(2 * i);
    t.insert(std::move(i));
  }

  std::vector<int> doubled;
  t.iterate([&](int x) { doubled.push_back(x * 2); });
  ASSERT_EQ(doubled, expected);

  auto double_fmap = t.fmap([](int x) { return x * 2; });
  ASSERT_EQ(double_fmap, expected);

  auto gathered = t.gather([](int x) { return x * 2; });
  std::sort(gathered.begin(), gathered.end());
  std::vector<int> double_gather(gathered.size());
  std::transform(gathered.begin(), gathered.end(), double_gather.begin(),
                 [](const auto& p) { return p.second; });
  ASSERT_EQ(double_gather, expected);
}

TEST(TreapTest, MergeTwoTreaps)
{
  Treap<int, std::less<int>> t1(std::less<int>{});
  Treap<int, std::less<int>> t2(std::less<int>{});

  t1.insert(1);
  t1.insert(3);
  t1.insert(5);

  t2.insert(2);
  t2.insert(4);
  t2.insert(6);

  t1.merge(std::move(t2));

  ASSERT_EQ(t1.size(), 6);

  std::vector<int> expected{1, 2, 3, 4, 5, 6};
  for (int i = 0; i < t1.size(); ++i) ASSERT_EQ(t1[i], expected[i]);
}

TEST(TreapTest, LargeInsertPerformance)
{
  Treap<int, std::less<int>> t(std::less<int>{});
  const int N = 10000;

  for (int i = 0; i < N; ++i) t.insert(std::move(i));

  ASSERT_EQ(t.size(), N);
  ASSERT_EQ(t[0], 0);
  ASSERT_EQ(t[N - 1], N - 1);
}

TEST(TreapTest, EdgeCasesEmptyTreap)
{
  Treap<int, std::less<int>> t(std::less<int>{});

  ASSERT_EQ(t.size(), 0);
  ASSERT_FALSE(t.contains(1));

  std::vector<int> v;
  t.iterate([&](int x) { v.push_back(x); });
  ASSERT_TRUE(v.empty());

  auto m = t.fmap([](int x) { return x * 2; });
  ASSERT_TRUE(m.empty());
}
