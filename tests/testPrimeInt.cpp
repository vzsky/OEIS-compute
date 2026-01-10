#include <gtest/gtest.h>
#include <stdexcept>
#include <utils/Prime.h>
#include <utils/PrimeInt.h>

TEST(PrimeIntTest, isDivisibleBy)
{
  {
    PrimeInt one{};

    PrimeInt a = 5;
    EXPECT_TRUE(a.is_divisible_by(one));
    EXPECT_TRUE(a.is_divisible_by(a));
    a = 10;
    EXPECT_TRUE(a.is_divisible_by(one));
    EXPECT_TRUE(a.is_divisible_by(a));
    a = 25;
    EXPECT_TRUE(a.is_divisible_by(one));
    EXPECT_TRUE(a.is_divisible_by(a));

    PrimeInt b = 25;
    PrimeInt c = 125;
    EXPECT_FALSE(b.is_divisible_by(c));

    c = 50;
    EXPECT_FALSE(b.is_divisible_by(c));
  }

  {
    PrimeInt b = 32;

    for (int i = 1; i <= 5; i++)
    {
      PrimeInt a = 1 << i;
      EXPECT_TRUE(b.is_divisible_by(a));
    }

    b = 32;

    for (int j = 11; j <= 20; j++)
    {
      EXPECT_TRUE((b * j).is_divisible_by(b));
      b *= j;
    }
  }
}

TEST(PrimeIntTest, Multiply)
{
  Prime<500> prime;

  PrimeInt one{};
  PrimeInt a(2);
  PrimeInt b(45);

  EXPECT_EQ(a * one, a);
  EXPECT_EQ(one * a, a);
  EXPECT_EQ(b * one, b);

  PrimeInt c(12);
  PrimeInt d(18);

  PrimeInt e = c * d;
  EXPECT_TRUE(e.is_divisible_by(c));
  EXPECT_TRUE(e.is_divisible_by(d));
  EXPECT_EQ(e, 216);

  EXPECT_EQ((a * b) * c, a * (b * c));
}

TEST(PrimeIntTest, Divide)
{
  Prime<500> prime;

  PrimeInt one{};
  PrimeInt a(60);

  EXPECT_EQ(a / one, a);

  PrimeInt b(72);
  PrimeInt c(12);

  PrimeInt d = b / c;
  EXPECT_EQ(c * d, b);
  EXPECT_EQ(d, 6);

  a = 14;
  b = 20;

  c = a * b;
  EXPECT_EQ(a, 14);
  EXPECT_EQ(b, 20);

  EXPECT_EQ(c, 14 * 20);
  c /= b;
  EXPECT_EQ(c, a);
  EXPECT_EQ(c / a, one);

  EXPECT_EQ(b / b, one);

  PrimeInt x = 2 * 3 * 5 * 7 * 11 * 13 * 17 * 19;
  PrimeInt y = 5 * 7 * 11 * 17 * 31;
  ASSERT_THROW(x / y, std::invalid_argument);
}
