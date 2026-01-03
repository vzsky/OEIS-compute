#include <gtest/gtest.h>
#include <utils/DecBigInt.h>

TEST(DecBigIntTest, Construction)
{
  DecBigInt a{};
  EXPECT_EQ(a, 0);

  DecBigInt b("12345");
  EXPECT_EQ(b, 12345);

  DecBigInt c("-987");
  EXPECT_EQ(c, -987);

  DecBigInt d("00123");
  EXPECT_EQ(d, DecBigInt(123));

  DecBigInt e("-00456");
  EXPECT_EQ(e, DecBigInt(-456));

  DecBigInt f("+789");
  EXPECT_EQ(f, DecBigInt(789));
}

TEST(DecBigIntTest, Comparison)
{
  DecBigInt a("123");
  DecBigInt b(123);
  DecBigInt c("456");
  DecBigInt d("-123");

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(a < c);
  EXPECT_TRUE(c > a);
  EXPECT_TRUE(d < a);
  EXPECT_TRUE(d < c);
}

TEST(DecBigIntTest, StreamOutput)
{
  DecBigInt a("00123");
  DecBigInt b("-0456");

  std::stringstream ss;
  ss << a << " " << b;
  EXPECT_EQ(ss.str(), "123 -456");
}

TEST(DecBigIntArithmeticTest, AdditionAssignment)
{
  DecBigInt a("123");

  a += DecBigInt(456);
  EXPECT_EQ(a, 579);

  DecBigInt b("-100");
  b += 50;
  EXPECT_EQ(b, DecBigInt(-50));

  DecBigInt c("0");
  c += DecBigInt(0);
  EXPECT_EQ(c, DecBigInt(0));
}

TEST(DecBigIntArithmeticTest, SubtractionAssignment)
{
  DecBigInt a("123");

  a -= DecBigInt(456);
  EXPECT_EQ(a, -333);

  DecBigInt b("-100");
  b -= 50;
  EXPECT_EQ(b, DecBigInt(-150));

  DecBigInt c("0");
  c -= DecBigInt(0);
  EXPECT_EQ(c, DecBigInt(0));
}

TEST(DecBigIntArithmeticTest, MultiplicationAssignment)
{
  DecBigInt a("123");

  a *= DecBigInt(456);
  EXPECT_EQ(a, 56088);

  DecBigInt b("-100");
  b *= -50;
  EXPECT_EQ(b, 5000);

  DecBigInt c("-1001");
  c *= 1234;
  EXPECT_EQ(c, -1235234);

  DecBigInt d("5");
  d *= DecBigInt(0);
  EXPECT_EQ(d, 0);

  DecBigInt e("1000000000");
  e *= e;
  e += 1;
  e *= 123456;
  EXPECT_EQ(e, std::string("123456000000000000123456"));
}
