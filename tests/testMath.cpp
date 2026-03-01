#include <gtest/gtest.h>
#include <math/Basic.h>
#include <math/Stats.h>
#include <utils/BigInt.h>

using namespace math;

TEST(BasicMath, PowBasic)
{
  EXPECT_EQ(pow(2, 0), 1);
  EXPECT_EQ(pow(2, 3), 8);
  EXPECT_EQ(pow(5, 4), 625);
  EXPECT_EQ(pow(1, 100), 1);
  EXPECT_EQ(pow(0, 0), 1);
  EXPECT_EQ(pow(0, 5), 0);
}

TEST(BasicMath, PowModBasic)
{
  EXPECT_EQ(pow(2, 0, 5), 1);
  EXPECT_EQ(pow(2, 3, 5), 3);
  EXPECT_EQ(pow(5, 4, 7), 2);
  EXPECT_EQ(pow(1, 100, 13), 1);
  EXPECT_EQ(pow(0, 0, 11), 1);
  EXPECT_EQ(pow(0, 5, 7), 0);
}

TEST(BasicMath, PowModLarge)
{
  EXPECT_EQ(pow(2, 30, 1000000007), 1073741824 % 1000000007);
  for (uint64_t a : {6, 1923, 5729, 9181}) EXPECT_EQ(pow<uint64_t>(a, 1e9 + 7, 1e9 + 7), a);
}

TEST(BasicMath, GCDBasic)
{
  EXPECT_EQ(gcd(10, 5), 5);
  EXPECT_EQ(gcd(14, 21), 7);
  EXPECT_EQ(gcd(17, 13), 1);
  EXPECT_EQ(gcd(0, 5), 5);
  EXPECT_EQ(gcd(5, 0), 5);
}

TEST(BasicMath, LCMBasic)
{
  EXPECT_EQ(lcm(4, 5), 20);
  EXPECT_EQ(lcm(6, 8), 24);
  EXPECT_EQ(lcm(21, 6), 42);
  EXPECT_EQ(lcm(0, 5), 0);
  EXPECT_EQ(lcm(5, 0), 0);
}

TEST(BasicMath, GCDBigInt)
{
  EXPECT_EQ(gcd<slow_bigint::DecBigInt>(14, 21), 7);
  EXPECT_EQ(gcd<slow_bigint::DecBigInt>(5, 0), 5);
}

TEST(BasicMath, LCMBigInt)
{
  EXPECT_EQ(lcm<slow_bigint::DecBigInt>(21, 6), 42);
  EXPECT_EQ(lcm<slow_bigint::DecBigInt>(0, 5), 0);
}

TEST(BasicMath, NCkBasic)
{
  EXPECT_EQ(nCk(5, 0), 1);
  EXPECT_EQ(nCk(5, 1), 5);
  EXPECT_EQ(nCk(5, 2), 10);
  EXPECT_EQ(nCk(5, 5), 1);
  EXPECT_EQ(nCk(0, 0), 1);
  EXPECT_EQ(nCk(10, 3), 120);

  EXPECT_EQ(nCk(10, 11), 0);
}

TEST(BasicMath, FactorialBasic)
{
  EXPECT_EQ(fact(0), 1);
  EXPECT_EQ(fact(1), 1);
  EXPECT_EQ(fact(2), 2);
  EXPECT_EQ(fact(3), 6);
  EXPECT_EQ(fact(4), 24);
  EXPECT_EQ(fact(5), 120);
  EXPECT_EQ(fact(10), 3628800);
  EXPECT_EQ(fact(12), 479001600);
}

TEST(MathStats, Average)
{
  int N = 100;
  std::vector<int> v;
  for (int i = 0; i < N; i++) v.push_back(20);
  EXPECT_EQ(stats::average(v), 20);
  for (int i = 0; i < N; i++) v.push_back(30);
  EXPECT_EQ(stats::average(v), 25);
  for (int i = 0; i < N; i++) v.push_back(40);
  EXPECT_EQ(stats::average(v), 30);
}

TEST(MathStats, Variance)
{
  const auto simple_test = [](int n)
  {
    std::vector<int> v = {1 + n, 2 + n, 3 + n};
    EXPECT_EQ(stats::variance(v), 2.0 / 3);
  };
  simple_test(0);
  simple_test(10);
  simple_test(50);
}
