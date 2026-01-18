#include <cstdint>
#include <gtest/gtest.h>
#include <numeric>
#include <utils/Fraction.h>

using Int = uint64_t;

TEST(FractionTest, Addition)
{
  Fraction<Int> a(1, 2);
  Fraction<Int> b(1, 3);

  auto c = a + b;

  EXPECT_EQ(c, Fraction<Int>(5, 6));
}

TEST(FractionTest, HarmonicAddition)
{
  Fraction<Int> a = 0;
  double expect   = 0;
  Int N           = 20;
  for (Int i = 1; i <= N; i++) a += Fraction<Int>{1, i};
  for (Int i = 1; i <= N; i++) expect += 1.0 / i;
  EXPECT_EQ(a.estimate(), expect);
}

TEST(FractionTest, Multiplication)
{
  Fraction<Int> a(2, 3);
  Fraction<Int> b(3, 4);

  auto c = a * b;
  c.normalize([](const Int& a, const Int& b) { return std::gcd(a, b); });

  EXPECT_EQ(c, Fraction<Int>(1, 2));
}
