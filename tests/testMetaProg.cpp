#include <gtest/gtest.h>
#include <utils/MetaProg.h>

TEST(MetaProgrammingTest, For)
{
  std::vector<int> output;

  const auto runTest = [&]<int A, int B>(auto&& f)
  {
    mp::For<A, B>(f);
    std::vector<int> output_mp = output;
    output.clear();
    for (int i = A; i < B; i++) f(i);
    ASSERT_EQ(output_mp, output);
    output.clear();
  };

  const auto sum = [&](int i) { output.push_back(i + (output.empty() ? 0 : output.back())); };

  const auto square = [&](int i) { output.push_back(i * i); };

  runTest.operator()<1, 5>(sum);
  runTest.operator()<10, 20>(sum);
  runTest.operator()<5, 15>(sum);
  runTest.operator()<7, 12>(square);
  runTest.operator()<13, 27>(square);
}
