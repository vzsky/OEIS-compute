#include <gtest/gtest.h>
#include <utils/ComplementNonnVector.h>

TEST(ComplementNonnVectorTest, EmptyComplement)
{
  ComplementNonnVector c;

  EXPECT_EQ(c[0], 0);
  for (int i = 55; i <= 555; i++) EXPECT_EQ(c[i], i);

  c.add_skip(0);
  EXPECT_EQ(c[0], 1);
  for (int i = 66; i <= 777; i++) EXPECT_EQ(c[i], i + 1);

  c.add_skip(1);
  c.add_skip(2);
  c.add_skip(3);
  c.add_skip(4);
  EXPECT_EQ(c[0], 5);
  for (int i = 999; i <= 1089; i++) EXPECT_EQ(c[i], i + 5);
}

TEST(ComplementNonnVectorTest, MultipleSkips)
{
  ComplementNonnVector c;
  c.add_skip(1);
  c.add_skip(3);
  c.add_skip(6);
  c.add_skip(8);
  // sequence: 0,2,4,5,7,9,10,...

  EXPECT_EQ(c[0], 0);
  EXPECT_EQ(c[1], 2);
  EXPECT_EQ(c[2], 4);
  EXPECT_EQ(c[3], 5);
  EXPECT_EQ(c[4], 7);
  EXPECT_EQ(c[5], 9);
}

TEST(ComplementNonnVectorTest, LargeValues)
{
  ComplementNonnVector c;
  c.add_skip(2);
  c.add_skip(1000000000000ULL);

  EXPECT_EQ(c[0], 0);
  EXPECT_EQ(c[1], 1);
  EXPECT_EQ(c[2], 3);
  EXPECT_EQ(c[999999999998ULL], 999999999999ULL);
  EXPECT_EQ(c[999999999999ULL], 1000000000001ULL);
}

TEST(ComplementNonnVectorTest, BinarySearch)
{
  ComplementNonnVector c;
  c.add_skip(1);
  c.add_skip(3);
  c.add_skip(6);
  c.add_skip(8);
  // sequence: 0,2,4,5,7,9,10,...

  const auto checkLowerbound = [&](uint64_t num)
  {
    auto lb = c.lower_bound(num);
    EXPECT_GE(c[lb], num);
    if (lb != 0) EXPECT_LT(c[lb - 1], num);
  };

  const auto checkUpperbound = [&](uint64_t num)
  {
    auto ub = c.upper_bound(num);
    EXPECT_GT(c[ub], num);
    if (ub != 0) EXPECT_LE(c[ub - 1], num);
  };

  for (int i = 0; i <= 10; i++) checkLowerbound(i);
}
