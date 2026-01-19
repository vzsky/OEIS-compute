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
  if constexpr (BI::Base == 10) EXPECT_EQ(ss.str(), "123 -456");
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

TYPED_TEST(BigIntTest, Addition)
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

TYPED_TEST(BigIntTest, Arithmetic)
{
  using BI = TypeParam;

  std::vector<int64_t> numbers = {123,   -100,   1001, 0,    1,       456,     -103,
                                  12345, -12345, 42,   5510, 1011001, 10523987};
  for (auto n : numbers)
  {
    for (auto m : numbers)
    {
      EXPECT_EQ(BI(n) + BI(m), BI(n + m));
      EXPECT_EQ(BI(n) - BI(m), BI(n - m));
      EXPECT_EQ(BI(n) * BI(m), BI(n * m));
    }
  }

  std::vector<int64_t> numerators   = {123, -103, 12345,      -12345,   52352333,
                                       42,  5510, 2391273124, 12318593, 126668449623};
  std::vector<int64_t> denominators = {5, 3, 2, 105, 3429, 2348242, -123123, -34, -789};
  for (auto n : numerators)
  {
    for (auto d : denominators)
    {
      EXPECT_EQ(BI(n) / BI(d), n / d);
      EXPECT_EQ(BI(n) % BI(d), n % d);
    }
  }
}

TYPED_TEST(BigIntTest, Multiplication)
{
  using BI = TypeParam;

  BI g(2);
  for (int i = 0; i < 100; i++) g *= 2;
  if constexpr (BI::Base == 10) EXPECT_EQ(g.digits().size(), 31);
}

TYPED_TEST(BigIntTest, Exponentiation)
{
  using BI = TypeParam;

  BI f(15);
  EXPECT_EQ(math::pow(f, 2), 225);

  BI g(1);
  for (int i = 0; i < 100; i++) g *= 2;

  BI h(2);
  h = math::pow(h, 100);

  EXPECT_EQ(h, g);
}

TEST(BigIntTest, BaseConversion)
{
  using DecBI   = BigInt<uint16_t, 10>;
  using BinBI   = BigInt<uint8_t, 2>;
  using HexBI   = BigInt<uint16_t, 16>;
  using DenseBI = DenseBigInt;

  for (auto n : std::vector<uint64_t>{1234285, 8234692643, 6529385728935})
  {
    DecBI dec(n);

    BinBI bin(dec);
    HexBI hex(dec);
    DecBI dec_from_bin(bin);
    DecBI dec_from_hex(hex);

    EXPECT_EQ(dec, dec_from_bin);
    EXPECT_EQ(dec, dec_from_hex);

    BinBI expect_bin(n);
    HexBI expect_hex(n);
    EXPECT_EQ(bin, expect_bin);
    EXPECT_EQ(hex, expect_hex);

    DenseBI dense(n);
    BinBI bin_from_dense(dense);
    DenseBI dense_from_bin_from_dense(bin_from_dense);

    EXPECT_EQ(bin, bin_from_dense);
    EXPECT_EQ(dense, dense_from_bin_from_dense);
  }
}

TEST(BigIntTest, MultOverflowBehavior)
{
  constexpr uint16_t Base = 128;
  constexpr int N         = 128;

  BigInt<uint16_t, Base> a = Base - 1;
  for (int i = 0; i < N; i++)
  {
    a *= Base;
    a += (Base - 1);
  }
  BigInt<uint16_t, Base> b = a;

  BigInt<uint16_t, Base> c = a * b;

  BigInt<uint64_t, Base> big_a = Base - 1;
  for (int i = 0; i < N; i++)
  {
    big_a *= Base;
    big_a += (Base - 1);
  }
  BigInt<uint64_t, Base> big_b = big_a;
  BigInt<uint64_t, Base> big_c = big_a * big_b;

  ASSERT_EQ(c.digits().size(), big_c.digits().size());
  for (int i = 0; i < c.digits().size(); i++) ASSERT_EQ((uint64_t)c.digits()[i], big_c.digits()[i]);
}

TEST(BigIntTest, DenseBigIntEdgeCase)
{
  DenseBigInt a(1);
  DecBigInt dec_a(1);
  DenseBigInt b(1);
  DecBigInt dec_b(1);
  DenseBigInt c(1);
  DecBigInt dec_c(1);

  for (int i = 1; i <= 100; i++)
  {
    c *= i;
    c -= 1;
    dec_c *= i;
    dec_c -= 1;
    a     = b * c + a;
    b     = a * c;
    dec_a = dec_b * dec_c + dec_a;
    dec_b = dec_a * dec_c;
  }

  EXPECT_EQ(a, DenseBigInt(dec_a));

  DenseBigInt top  = 1;
  DenseBigInt fact = 1;
  DenseBigInt bot  = 1;

  for (int k = 2; k <= 100; ++k)
  {
    fact *= k;
    DenseBigInt d = fact - 1;

    top = top * d + bot;
    bot = bot * d;

    auto dtop  = DecBigInt(top);
    auto dfact = DecBigInt(fact);
    auto dbot  = DecBigInt(bot);

    EXPECT_EQ(DecBigInt(top), dtop);
    EXPECT_EQ(top, DenseBigInt(dtop));
    EXPECT_EQ(DecBigInt(fact), dfact);
    EXPECT_EQ(fact, DenseBigInt(dfact));
    EXPECT_EQ(DecBigInt(bot), dbot);
    EXPECT_EQ(bot, DenseBigInt(dbot));
  }
}
