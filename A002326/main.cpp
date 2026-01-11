#include "lib.h"
#include <utils/Utils.h>

int main()
{
  utils::timeit([]()
  {
    constexpr int N = 1e5;
    A002326<N> a;
    for (int i = 1; i < N; i++) a.get_answer(i);
  });

  utils::timeit([]()
  {
    constexpr int N = 1e5;
    A002326<N> a;
    auto res = a.get_answers_until(N);
  });
}
