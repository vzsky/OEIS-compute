#include <A045345/lib.hpp>
#include <utils/Utils.hpp>

int main()
{
  constexpr int N         = 1e5;
  std::vector<int> result = A045345::answers_upto<N>();
  for (int x : result) Log(Info, x);
}
