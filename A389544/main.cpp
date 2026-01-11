#include "lib.h"

struct A389544_impl : public A389544<500000, 2'000'000'000>
{
  void _not_skip(uint64_t n) override { std::cout << n << std::endl; }
};

int main()
{
  utils::timeit([]
  {
    A389544_impl e;
    e.get_sequence_until_N();
  });
}
