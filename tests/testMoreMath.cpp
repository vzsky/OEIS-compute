#include <gtest/gtest.h>
#include <utils/BigInt.h>
#include <utils/MoreMath.h>

using namespace math;

TEST(MoreMathTest, PowBasic)
{
  EXPECT_EQ(pow(2, 0), 1);
  EXPECT_EQ(pow(2, 3), 8);
  EXPECT_EQ(pow(5, 4), 625);
  EXPECT_EQ(pow(1, 100), 1);
  EXPECT_EQ(pow(0, 0), 1);
  EXPECT_EQ(pow(0, 5), 0);
}

TEST(MoreMathTest, PowModBasic)
{
  EXPECT_EQ(pow(2, 0, 5), 1);
  EXPECT_EQ(pow(2, 3, 5), 3);
  EXPECT_EQ(pow(5, 4, 7), 2);
  EXPECT_EQ(pow(1, 100, 13), 1);
  EXPECT_EQ(pow(0, 0, 11), 1);
  EXPECT_EQ(pow(0, 5, 7), 0);
}

TEST(MoreMathTest, PowModLarge)
{
  EXPECT_EQ(pow(2, 30, 1000000007), 1073741824 % 1000000007);
  for (uint64_t a : {6, 1923, 5729, 9181}) EXPECT_EQ(pow<uint64_t>(a, 1e9 + 7, 1e9 + 7), a);
}

TEST(MoreMathTest, GCDBasic)
{
  EXPECT_EQ(gcd(10, 5), 5);
  EXPECT_EQ(gcd(14, 21), 7);
  EXPECT_EQ(gcd(17, 13), 1);
  EXPECT_EQ(gcd(0, 5), 5);
  EXPECT_EQ(gcd(5, 0), 5);
}

TEST(MoreMathTest, LCMBasic)
{
  EXPECT_EQ(lcm(4, 5), 20);
  EXPECT_EQ(lcm(6, 8), 24);
  EXPECT_EQ(lcm(21, 6), 42);
  EXPECT_EQ(lcm(0, 5), 0);
  EXPECT_EQ(lcm(5, 0), 0);
}

TEST(MoreMathTest, GCDBigInt)
{
  EXPECT_EQ(gcd<DecBigInt>(14, 21), 7);
  EXPECT_EQ(gcd<DecBigInt>(5, 0), 5);
}

TEST(MoreMathTest, LCMBigInt)
{
  EXPECT_EQ(lcm<DecBigInt>(21, 6), 42);
  EXPECT_EQ(lcm<DecBigInt>(0, 5), 0);
}
