#include <gtest/gtest.h>
#include <utils/ComplementNonnVector.h>

using namespace complementNonnVector;

TEST(ComplementNonnVectorTest, EmptyComplement)
{
  Vector c;

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
  Vector c;
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
  Vector c;
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
  Vector c;
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

class ComplementNonnVectorIteratorTest : public ::testing::Test
{
protected:
  Vector v;

  void SetUp() override
  {
    v.add_skip(2);
    v.add_skip(5);
    v.add_skip(7);
    // sequence: 0,1,3,4,6,8,9,...
  }
};

TEST_F(ComplementNonnVectorIteratorTest, ConstructFromIndex)
{
  Vector::iterator it(&v, 0);
  EXPECT_EQ(*it, 0);

  Vector::iterator it2(&v, 2);
  EXPECT_EQ(*it2, 3);

  Vector::iterator it3(&v, 4);
  EXPECT_EQ(*it3, 6);
}

TEST_F(ComplementNonnVectorIteratorTest, ForwardIterationSkipsComplement)
{
  Vector::iterator it(&v, 0);

  EXPECT_EQ(*it, 0);
  ++it;
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(*it, 6);
  ++it;
  EXPECT_EQ(*it, 8);
}

TEST_F(ComplementNonnVectorIteratorTest, BackwardIterationSkipsComplement)
{
  Vector::iterator it(&v, 5); // value = 8

  EXPECT_EQ(*it, 8);
  --it;
  EXPECT_EQ(*it, 6);
  --it;
  EXPECT_EQ(*it, 4);
  --it;
  EXPECT_EQ(*it, 3);
  --it;
  EXPECT_EQ(*it, 1);
  --it;
  EXPECT_EQ(*it, 0);
}

TEST_F(ComplementNonnVectorIteratorTest, EqualityAndInequality)
{
  Vector::iterator a(&v, 3);
  Vector::iterator b(&v, 3);
  Vector::iterator c(&v, 4);

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(a == c);
}

TEST_F(ComplementNonnVectorIteratorTest, Looping)
{
  std::vector<uint64_t> out;
  for (auto it = v.it_at(5); it.idx() >= 0; --it) out.push_back(*it);
  EXPECT_EQ(out, std::vector<uint64_t>({8, 6, 4, 3, 1, 0}));

  out.clear();
  for (auto it = v.it_at(5); *it >= 1; --it) out.push_back(*it);
  EXPECT_EQ(out, std::vector<uint64_t>({8, 6, 4, 3, 1}));

  out.clear();
  for (auto it = v.it_at(0); *it <= 5; ++it) out.push_back(*it);
  EXPECT_EQ(out, std::vector<uint64_t>({0, 1, 3, 4}));

  out.clear();
  for (auto it = v.it_at(0); it.idx() <= 5; ++it) out.push_back(*it);
  EXPECT_EQ(out, std::vector<uint64_t>({0, 1, 3, 4, 6, 8}));
}
