#include <A389544/lib.hpp>
#include <cassert>
#include <cstdint>
#include <utils/Utils.hpp>

constexpr uint32_t CacheLim = 2e9;

template <uint32_t N> struct A390848 : public A389544<N, CacheLim>
{
  void _skip(uint64_t n) override
  {
    static int ind = 1;
    Log(LL::Info, ind++, n);
    if (this->is_interesting(n)) Log(LL::Info, "INTERESTING SKIP $"_f, n);
  }
};

int main()
{
  // {
  //   utils::ScopeTimer _t{};
  //   A390848<101'000'000> e{};
  //   e.get_sequence_until_N();
  // }

  {
    utils::ScopeTimer _t{};
    A390848<101'000'000> e{};
    // e.read_skipped(utils::read_bfile<uint64_t>("./b390848.txt"));
    e.get_sequence_until_N();
  }
}
