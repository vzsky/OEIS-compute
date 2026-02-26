#include <gtest/gtest.h>
#include <utils/Fft.h>

using namespace fft;

constexpr double DOUBLE_COMPARE_EPS = 1e-10;

TEST(FftTest, TransformBack)
{
  const auto doTest = [](std::vector<cd> original)
  {
    std::vector<cd> trans = original;
    transform<Direction::Forward>(trans);
    transform<Direction::Inverse>(trans);

    for (size_t i = 0; i < original.size(); ++i)
    {
      EXPECT_NEAR(original[i].real(), trans[i].real(), DOUBLE_COMPARE_EPS);
      EXPECT_NEAR(original[i].imag(), trans[i].imag(), DOUBLE_COMPARE_EPS);
    }
  };

  doTest({1, 2, 3, 4, 5, 6, 7, 8});
  doTest({1, 0, 0, 1});
  doTest({1, 5});
  doTest({1, 4, 4, 9});
  doTest({{1, 2}, {3, 4}, {5, 6}, {7, 8}});
}

TEST(FftTest, ConvolutionBasic)
{
  std::vector<cd> a = {1, 2, 3};
  std::vector<cd> b = {4, 5};

  auto c = convolution(a, b);

  std::vector<cd> expected = {
      1 * 4,         //
      1 * 5 + 2 * 4, //
      2 * 5 + 3 * 4, //
      3 * 5          //
  };

  ASSERT_EQ(c.size(), expected.size());

  for (size_t i = 0; i < c.size(); ++i)
  {
    EXPECT_NEAR(c[i].real(), expected[i].real(), DOUBLE_COMPARE_EPS);
    EXPECT_NEAR(c[i].imag(), expected[i].imag(), DOUBLE_COMPARE_EPS);
  }
}

TEST(FftTest, ConvolutionLarge)
{
  constexpr double eps = 1e-10;

  const int n = 128;
  const int m = 96;

  std::vector<cd> a(n), b(m);
  for (int i = 0; i < n; ++i) a[i] = cd(i % 7 - 3, i % 5 - 2);
  for (int j = 0; j < m; ++j) b[j] = cd(j % 11 - 5, j % 3 - 1);

  auto c = convolution(a, b);

  std::vector<cd> expected(n + m - 1, cd(0, 0));
  for (int i = 0; i < n; ++i)
  {
    for (int j = 0; j < m; ++j)
    {
      expected[i + j] += a[i] * b[j];
    }
  }

  ASSERT_EQ(c.size(), expected.size());

  for (size_t i = 0; i < c.size(); ++i)
  {
    EXPECT_NEAR(c[i].real(), expected[i].real(), eps);
    EXPECT_NEAR(c[i].imag(), expected[i].imag(), eps);
  }
}
