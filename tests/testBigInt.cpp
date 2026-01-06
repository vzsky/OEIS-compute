#include <gtest/gtest.h>
#include <utils/BigInt.h>

template <typename T> class BigIntTest : public ::testing::Test
{
};

using BigIntTypes = ::testing::Types<DecBigInt, DenseBigInt>;
TYPED_TEST_SUITE(BigIntTest, BigIntTypes);

TYPED_TEST(BigIntTest, Construction)
{
  using BI = TypeParam;

  EXPECT_EQ(BI{}, 0);
  EXPECT_EQ(BI("12345"), BI(12345));
  EXPECT_EQ(BI("-987"), BI(-987));
  EXPECT_EQ(BI("000123"), BI(123));
  EXPECT_EQ(BI("-00456"), BI(-456));
  EXPECT_EQ(BI("+78900"), BI(78900));
}

TYPED_TEST(BigIntTest, StreamOutput)
{
  using BI = TypeParam;

  std::stringstream ss;
  ss << BI("000123") << " " << BI(-456);
  if constexpr (BI::Base == 10)
    EXPECT_EQ(ss.str(), "(1)(2)(3) -(4)(5)(6)");
  else
    EXPECT_EQ(ss.str(), "(123) -(1)(200)");
}

TYPED_TEST(BigIntTest, Comparison)
{
  using BI = TypeParam;

  BI a(123);
  BI b(123);
  BI c(456);
  BI d(-123);

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a < c);
  EXPECT_TRUE(c > a);
  EXPECT_TRUE(d < a);
  EXPECT_TRUE(d < c);
  EXPECT_TRUE(a == -d);
  EXPECT_TRUE(-a == d);
}

TYPED_TEST(BigIntTest, AdditionAssignment)
{
  using BI = TypeParam;

  BI a(123);
  a += BI(456);
  EXPECT_EQ(a, 579);

  BI b(-100);
  b += 50;
  EXPECT_EQ(b, -50);

  BI c(0);
  c += BI(0);
  EXPECT_EQ(c, 0);
}

TYPED_TEST(BigIntTest, SubtractionAssignment)
{
  using BI = TypeParam;

  BI a(123);
  a -= BI(456);
  EXPECT_EQ(a, -333);

  BI b(-100);
  b -= 50;
  EXPECT_EQ(b, -150);

  BI c(0);
  c -= BI(0);
  EXPECT_EQ(c, 0);
}

TYPED_TEST(BigIntTest, MultiplicationAssignment)
{
  using BI = TypeParam;

  BI a(123);
  a *= BI(456);
  EXPECT_EQ(a, 56088);

  BI b(-100);
  b *= -50;
  EXPECT_EQ(b, 5000);

  BI c(-1001);
  c *= 1234;
  EXPECT_EQ(c, -1235234);

  BI d(5);
  d *= BI(0);
  EXPECT_EQ(d, 0);

  BI e(1'000'000'000);
  e *= e;
  e += 1;
  e *= 123456;

  BI f(123456);
  for (int i = 0; i < 18; i++)
    f *= 10;
  f += 123456;
  EXPECT_EQ(e, f);
}

TYPED_TEST(BigIntTest, ModuloAssignment)
{
  using BI = TypeParam;

  BI a(123);
  a %= 5;
  EXPECT_EQ(a, 3);

  BI b(-103);
  b %= 5;
  EXPECT_EQ(b, 2);

  BI c(12345);
  c %= 123;
  EXPECT_EQ(c, 12345 % 123);

  BI d(-12345);
  d %= 123;
  EXPECT_EQ(d, 123 - (12345 % 123));

  BI e(98765);
  e %= 1;
  EXPECT_EQ(e, 0);

  BI f(42);
  f %= 100;
  EXPECT_EQ(f, 42);
}
