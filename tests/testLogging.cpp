#include <array>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <string_view>
#include <utils/Logging.hpp>
#include <vector>

namespace {

std::vector<std::string> gCaptured;
void captureSink(std::string_view msg) { gCaptured.emplace_back(msg); }
using _LogEnv_ = _LogEnv_::Logger<captureSink>;

struct Capture
{
  std::size_t pos = 0;

  Capture() { gCaptured.clear(); }

  void expect(std::string_view expected)
  {
    ASSERT_LT(pos, gCaptured.size()) << "no more output";
    std::string_view line = gCaptured[pos++];
    while (!line.empty() && line.back() == ' ') line.remove_suffix(1);
    EXPECT_EQ(line, expected);
  }

  bool empty() const { return gCaptured.empty(); }
};

TEST(Logging, BareOutput)
{
  Capture cap;
  Log(Info, "bare");
  cap.expect("bare");
}

TEST(Logging, ModuleAppearsInBrackets)
{
  Capture cap;
  using _LogEnv_ = _LogEnv_::Module<"server">;
  Log(Info, "started");
  cap.expect("[server] started");
}

TEST(Logging, NestedModuleAppends)
{
  Capture cap;
  using _LogEnv_ = _LogEnv_::Module<"server">;
  {
    using _LogEnv_ = _LogEnv_::Module<"db">;
    Log(Info, "connected");
  }
  cap.expect("[server::db] connected");
}

TEST(Logging, ScopeExitRestoresModule)
{
  Capture cap;
  {
    using _LogEnv_ = _LogEnv_::Module<"inner">;
    Log(Info, "inside");
  }
  Log(Info, "outside");
  cap.expect("[inner] inside");
  cap.expect("outside");
}

TEST(Logging, DebugSilencedAtInfoThreshold)
{
  Capture cap;
  Log(Debug, "should not appear");
  EXPECT_TRUE(cap.empty());
}

TEST(Logging, LevelFiltering)
{
  Capture cap;
  using _LogEnv_ = _LogEnv_::Level<LL::Warn>;
  Log(Info, "silenced");
  Log(Warn, "warn fires");
  Log(Error, "error fires");
  cap.expect("warn fires");
  cap.expect("error fires");
}

TEST(Logging, ModuleAndLevelChained)
{
  Capture cap;
  using _LogEnv_ = _LogEnv_::Module<"quiet">::Level<LL::Warn>;
  Log(Info, "silenced");
  Log(Warn, "fires");
  cap.expect("[quiet] fires");
}

TEST(Logging, RangeVector)
{
  Capture cap;
  std::vector<int> v = {1, 2, 3};
  Log(Info, v);
  cap.expect("{1, 2, 3}");
}

TEST(Logging, RangeArray)
{
  Capture cap;
  std::array<int, 3> a = {10, 20, 30};
  Log(Info, a);
  cap.expect("{10, 20, 30}");
}

TEST(Logging, RangeList)
{
  Capture cap;
  std::list<std::string> l = {"foo", "bar"};
  Log(Info, l);
  cap.expect("{foo, bar}");
}

TEST(Logging, StringNotTreatedAsRange)
{
  Capture cap;
  std::string s1      = "hello";
  std::string_view s2 = "hello";
  char s3[]           = "hello";
  auto s4             = "hello"_ms;
  Log(Info, s1);
  Log(Info, s2);
  Log(Info, s3);
  Log(Info, s4);
  cap.expect("hello");
  cap.expect("hello");
  cap.expect("hello");
  cap.expect("hello");
}

TEST(Logging, LogFBasic)
{
  Capture cap;
  LogF(Info, "x=$ y=$", 3, 4);
  cap.expect("x=3 y=4");
}

TEST(Logging, LogFRange)
{
  Capture cap;
  std::vector<int> v = {1, 2, 3};
  LogF(Info, "v=$", v);
  cap.expect("v={1, 2, 3}");
}

// TODO can we have this
void f() { Log(Debug, "downstream"); }
TEST(Logging, LogDeclare)
{
  Capture cap;
  using _LogEnv_ = _LogEnv_::Level<LL::Debug>;
  f();
  cap.expect("downstream");
}

} // namespace
