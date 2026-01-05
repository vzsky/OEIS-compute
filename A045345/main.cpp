#include "lib.h"
#include <iostream>
#include <utils/Utils.h>

int main()
{
  constexpr int N = 1e5;
  std::vector<int> result = A045345::answers_upto<N>();
  for (auto x : result)
  {
    std::cout << x << std::endl;
  }
}
