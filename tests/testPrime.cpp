#include <gtest/gtest.h>
#include <stdexcept>
#include <utils/Prime.h>

using prime = size_t;

TEST(PrimeTest, SmallNumbers)
{
  Prime<20> pf{};

  auto factors7 = pf.factors(7);
  ASSERT_EQ(factors7.size(), 1);
  EXPECT_EQ(factors7[0], 7);

  auto freq7 = pf.factors_freq(7);
  ASSERT_EQ(freq7.size(), 1);
  EXPECT_EQ(freq7[7], 1);

  auto factors12                = pf.factors(12);
  std::vector<prime> expected12 = {2, 2, 3};
  EXPECT_EQ(factors12, expected12);

  auto freq12 = pf.factors_freq(12);
  ASSERT_EQ(freq12.size(), 2);
  EXPECT_EQ(freq12[2], 2);
  EXPECT_EQ(freq12[3], 1);

  auto factors18                = pf.factors(18);
  std::vector<prime> expected18 = {2, 3, 3};
  EXPECT_EQ(factors18, expected18);

  auto freq18 = pf.factors_freq(18);
  ASSERT_EQ(freq18.size(), 2);
  EXPECT_EQ(freq18[2], 1);
  EXPECT_EQ(freq18[3], 2);
}

TEST(PrimeTest, EdgeCases)
{
  Prime<10> pf;

  auto factors2 = pf.factors(2);
  ASSERT_EQ(factors2.size(), 1);
  EXPECT_EQ(factors2[0], 2);

  auto factors10                = pf.factors(10);
  std::vector<prime> expected10 = {2, 5};
  EXPECT_EQ(factors10, expected10);

  auto freq10 = pf.factors_freq(10);
  ASSERT_EQ(freq10.size(), 2);
  EXPECT_EQ(freq10[2], 1);
  EXPECT_EQ(freq10[5], 1);
}

TEST(PrimeTest, WorksForOutOfRange)
{
  Prime<10> pf;

  ASSERT_TRUE(pf.factors(1).empty());
  ASSERT_EQ(pf.factors(11), std::vector<prime>{11});
  ASSERT_EQ(pf.factors(15), std::vector<prime>({3, 5}));
  ASSERT_EQ(pf.factors(50), std::vector<prime>({2, 5, 5}));
  ASSERT_EQ(pf.factors(11 * 11), std::vector<prime>({11, 11}));
  ASSERT_EQ(pf.factors(11 * 17), std::vector<prime>({11, 17}));
}

TEST(PrimeTest, IsPrime)
{
  Prime<100> pf;

  ASSERT_THROW(pf.is_prime(0), std::invalid_argument);
  ASSERT_FALSE(pf.is_prime(1));
  ASSERT_TRUE(pf.is_prime(2));
  ASSERT_TRUE(pf.is_prime(19));
  ASSERT_FALSE(pf.is_prime(100));
  ASSERT_FALSE(pf.is_prime(1025));
  ASSERT_FALSE(pf.is_prime(1027));
  ASSERT_FALSE(pf.is_prime(10201));
  ASSERT_TRUE(pf.is_prime(17));
  ASSERT_TRUE(pf.is_prime(7027));
  ASSERT_TRUE(pf.is_prime(8011));
  ASSERT_TRUE(pf.is_prime(121021));
  ASSERT_TRUE(pf.is_prime(1e9 + 7));
}
