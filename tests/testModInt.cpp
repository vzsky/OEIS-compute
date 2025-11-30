#include <gtest/gtest.h>
#include <utils/ModInt.h>

TEST(ModIntTest, Construction)
{
  ModInt<11> a{0};
  EXPECT_EQ(a, 0);

  ModInt<11> b{1};
  EXPECT_EQ(b, 1);

  ModInt<11> c{37};
  EXPECT_EQ(c, 4);
}
