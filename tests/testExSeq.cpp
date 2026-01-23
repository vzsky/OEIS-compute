#include <A002326/lib.h>
#include <A045345/lib.h>
#include <A331373/lib.h>
#include <gtest/gtest.h>
#include <vector>

TEST(TestExSeq, testA002326)
{
  constexpr int N = 10;
  A002326<N> a{};
  std::vector<int> result;
  for (int i = 2; i <= N; i++)
  {
    result.push_back(a.get_answer(i));
  }
  std::vector<int> range_result = a.get_answers_until(N);
  EXPECT_EQ(result, std::vector({2, 4, 3, 6, 10, 12, 4, 8, 18}));
  EXPECT_EQ(result, range_result);
}

TEST(TestExSeq, testA045345)
{
  constexpr int N = 1000;
  auto answers    = A045345::answers_upto<N>();
  EXPECT_EQ(answers, std::vector({1, 23, 53}));
}

TEST(TestExSeq, testA331373)
{
  auto fraction = A331373::get_fraction<150>();
  auto mantissa = fraction.expansion<DecBigInt>(70);
  auto answers  = mantissa.digits();
  std::reverse(begin(answers), end(answers));
  EXPECT_EQ(answers,
            std::vector<uint16_t>({1, 2, 5, 3, 4, 9, 8, 7, 5, 5, 6, 9, 9, 9, 5, 3, 4, 7, 1, 6, 4, 3, 3, 6,
                                   0, 9, 3, 7, 9, 0, 5, 7, 9, 8, 9, 4, 0, 3, 6, 9, 2, 3, 2, 2, 0, 8, 3, 3,
                                   2, 0, 1, 3, 4, 1, 7, 0, 6, 3, 8, 3, 4, 7, 1, 6, 6, 4, 0, 9, 5, 2, 4}));
}
