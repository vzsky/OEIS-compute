#include "lib.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <utils/Utils.h>

namespace
{

double DOUBLE_COMPARE_EPS = 1e-10;
inline bool feq(double a, double b) { return std::abs(a - b) < DOUBLE_COMPARE_EPS; }

double PHI     = 1.618033988749894848204;
double INV_PHI = 1 / PHI;

int A371627(int n)
{
  if (n == 1) return 0;
  int answer     = -1;
  double current = 100;
  for (int x = 0; x < n; x++)
  {
    int y       = n - x;
    double cand = (double)x / y;
    if (std::abs(cand - INV_PHI) < current)
    {
      current = std::abs(cand - INV_PHI);
      answer  = x;
    }
  }
  return answer;
}

int A371627_comment(std::int64_t n)
{
  const double target = static_cast<double>(n) / (PHI * PHI);

  const std::int64_t x1 = static_cast<int>(std::floor(target));
  const std::int64_t x2 = static_cast<int>(std::ceil(target));

  auto err = [&](std::int64_t x)
  {
    if (x <= 0 || x >= n) return std::numeric_limits<double>::infinity();
    return std::abs(static_cast<double>(x) / (n - x) - INV_PHI);
  };

  if (feq(err(x1), err(x2)))
  {
    std::cout << "careful with n=" << n << std::endl;
    assert(false);
  }
  return (err(x1) <= err(x2)) ? x1 : x2;
}

} // namespace

std::vector<int> solutions = {0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  5,  6,  6,  6,  7,
                              7,  8,  8,  8,  9,  9,  10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14,
                              14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21,
                              21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27};

int main()
{
  assert(feq((1.0 + std::sqrt(5.0)) / 2.0, PHI));
  for (int i = 1; i < solutions.size(); i++) assert(A371627(i) == solutions[i - 1]);

  // manually checked
  // std::unordered_map<int,int> manual;
  // manual[5473] = 2090;
  // manual[8057] = 3078;
  // manual[9654] = 3687;

  for (int i = 1; i < 20001; i++)
  {
    std::cout << i << " " << A371627(i) << std::endl;
    assert(A371627(i) == A371627_comment(i));
  }
}
