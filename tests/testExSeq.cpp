#include <A002326/lib.h>
#include <A045345/lib.h>
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
