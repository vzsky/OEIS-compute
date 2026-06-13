#include "Utils.hpp"
#include <A389544/lib.hpp>

struct A389544_impl : public A389544<500000, 2'000'000'000>
{
  void _not_skip(uint64_t n) override { Log(Info, n); }
};

int main()
{
  {
    utils::ScopeTimer _t{};
    A389544_impl e;
    e.get_sequence_until_N();
  }
}
