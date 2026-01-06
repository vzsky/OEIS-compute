#include "lib.h"

int main()
{
  A389544<25000> e;
  const std::vector<uint64_t> &result = e.get_sequence_until_N();
  size_t index                        = 0;
  for (auto x : result)
  {
    std::cout << ++index << ' ' << x << std::endl;
  }
}
