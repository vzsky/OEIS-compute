#include "lib.hpp"
#include <iostream>
#include <utils/Utils.hpp>

int main()
{
  constexpr int N         = 1e5;
  std::vector<int> result = A045345::answers_upto<N>();
  for (auto x : result)
  {
    std::cout << x << std::endl;
  }
}
