#include <gtest/gtest.h>
#include <utils/BigInt.h>

TEST(BigIntTest, Construction)
{
  BigInt a{};
  EXPECT_EQ(a, 0);

  BigInt b("12345");
  EXPECT_EQ(b, 12345);

  BigInt c("-987");
  EXPECT_EQ(c, -987);

  BigInt d("00123");
  EXPECT_EQ(d, BigInt(123));

  BigInt e("-00456");
  EXPECT_EQ(e, BigInt(-456));

  BigInt f("+789");
  EXPECT_EQ(f, BigInt(789));
}

TEST(BigIntTest, Comparison)
{
  BigInt a("123");
  BigInt b(123);
  BigInt c("456");
  BigInt d("-123");

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a < c);
  EXPECT_TRUE(c > a);
  EXPECT_TRUE(d < a);
  EXPECT_TRUE(d < c);
}

TEST(BigIntTest, StreamOutput)
{
  BigInt a("00123");
  BigInt b("-0456");

  std::stringstream ss;
  ss << a << " " << b;
  EXPECT_EQ(ss.str(), "123 -456");
}

TEST(BigIntArithmeticTest, AdditionAssignment)
{
  BigInt a("123");

  a += BigInt(456);
  EXPECT_EQ(a, 579);

  BigInt b("-100");
  b += 50;
  EXPECT_EQ(b, BigInt(-50));

  BigInt c("0");
  c += BigInt(0);
  EXPECT_EQ(c, BigInt(0));
}

TEST(BigIntArithmeticTest, SubtractionAssignment)
{
  BigInt a("123");

  a -= BigInt(456);
  EXPECT_EQ(a, -333);

  BigInt b("-100");
  b -= 50;
  EXPECT_EQ(b, BigInt(-150));

  BigInt c("0");
  c -= BigInt(0);
  EXPECT_EQ(c, BigInt(0));
}

TEST(BigIntArithmeticTest, MultiplicationAssignment)
{
  BigInt a("123");

  a *= BigInt(456);
  EXPECT_EQ(a, 56088);

  BigInt b("-100");
  b *= -50;
  EXPECT_EQ(b, 5000);

  BigInt c("-1001");
  c *= 1234;
  EXPECT_EQ(c, -1235234);

  BigInt d("5");
  d *= BigInt(0);
  EXPECT_EQ(d, 0);

  BigInt e("1000000000");
  e *= e;
  e += 1;
  e *= 123456;
  EXPECT_EQ(e, std::string("123456000000000000123456"));
}
