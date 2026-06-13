#include <array>
#include <gtest/gtest.h>
#include <list>
#include <string>
#include <string_view>
#include <thread>
#include <utils/Logging.hpp>
#include <vector>

namespace {

void debugDownstream() { Log(LL::Debug, "downstream"); }
void infoHello() { Log(LL::Info, "hi"); }

struct Capture
{
  logging::Scope _l = logging::Env{}.logger(captureLog);

  static std::vector<std::string> captured;
  static void tlCaptureWrite(std::string_view msg) { captured.emplace_back(msg); }
  constexpr static logging::Logger captureLog{&tlCaptureWrite};

  std::size_t pos = 0;

  Capture() { captured.clear(); }
  ~Capture() { EXPECT_TRUE(empty()); }

  void expect(std::string_view expected)
  {
    ASSERT_LT(pos, captured.size()) << "no more output";
    std::string_view line = captured[pos++];
    while (!line.empty() && line.back() == ' ') line.remove_suffix(1);
    EXPECT_EQ(line, expected);
  }

  bool empty() const { return pos == captured.size(); }
};
std::vector<std::string> Capture::captured{};

} // namespace

TEST(Logging, BareOutput)
{
  Capture cap;
  Log(LL::Info, "bare");
  cap.expect("bare");
}

TEST(Logging, ModuleAppearsInBrackets)
{
  Capture cap;
  logging::Scope s = logging::Env{}.module("server");
  Log(LL::Info, "started");
  cap.expect("[server] started");
}

TEST(Logging, NestedModuleAppends)
{
  Capture cap;
  logging::Scope s = logging::Env{}.module("server");
  {
    logging::Scope inner = logging::Env{}.module("db");
    Log(LL::Info, "connected");
  }
  cap.expect("[server::db] connected");
}

TEST(Logging, ScopeExitRestoresModule)
{
  Capture cap;
  {
    logging::Scope s = logging::Env{}.module("inner");
    Log(LL::Info, "inside");
  }
  Log(LL::Info, "outside");
  cap.expect("[inner] inside");
  cap.expect("outside");
}

TEST(Logging, DebugSilencedAtInfoThreshold)
{
  Capture cap;
  Log(LL::Debug, "should not appear");
  EXPECT_TRUE(cap.empty());
}

TEST(Logging, LevelFiltering)
{
  Capture cap;
  logging::Scope s = logging::Env{}.level(LL::Warn);
  Log(LL::Info, "silenced");
  Log(LL::Warn, "warn fires");
  Log(LL::Error, "error fires");
  cap.expect("warn fires");
  cap.expect("error fires");
}

TEST(Logging, ModuleAndLevelChained)
{
  Capture cap;
  logging::Scope s = logging::Env{}.module("quiet").level(LL::Warn);
  Log(LL::Info, "silenced");
  Log(LL::Warn, "fires");
  cap.expect("[quiet] fires");
}

TEST(Logging, EnvPropagatesToCallee)
{
  Capture cap;
  {
    logging::Scope s = logging::Env{}.level(LL::Debug);
    debugDownstream();
    cap.expect("downstream");
  }
  {
    logging::Scope s = logging::Env{}.level(LL::Info);
    debugDownstream();
    EXPECT_TRUE(cap.empty());
  }
}

TEST(Logging, ModulePropagatesToCallee)
{
  Capture cap;
  {
    logging::Scope s = logging::Env{}.module("svc");
    infoHello();
    cap.expect("[svc] hi");
  }
  {
    logging::Scope s = logging::Env{}.module("nice logger");
    infoHello();
    cap.expect("[nice logger] hi");
  }
}

TEST(Logging, printRanges)
{
  Capture cap;
  std::vector<int> v = {1, 2, 3};
  Log(LL::Info, v);
  cap.expect("{1, 2, 3}");

  std::array<int, 3> a = {10, 20, 30};
  Log(LL::Info, a);
  cap.expect("{10, 20, 30}");

  std::span<int> s = v;
  Log(LL::Info, s);
  cap.expect("{1, 2, 3}");
}

TEST(Logging, RangeList)
{
  Capture cap;
  std::list<std::string> l = {"foo", "bar"};
  Log(LL::Info, l);
  cap.expect("{foo, bar}");
}

TEST(Logging, StringNotTreatedAsRange)
{
  Capture cap;
  std::string s1      = "hello";
  std::string_view s2 = "hello";
  char s3[]           = "hello";
  auto s4             = "hello"_ms;
  Log(LL::Info, s1, s2, s3, s4);
  cap.expect("hello hello hello hello");
}

TEST(Logging, LogFBasic)
{
  Capture cap;
  Log(LL::Info, "x=$ y=$"_f, 3, 4);
  cap.expect("x=3 y=4");
}

TEST(Logging, LogFRange)
{
  Capture cap;
  std::vector<int> v = {1, 2, 3};
  Log(LL::Info, "v=$"_f, v);
  cap.expect("v={1, 2, 3}");
}

TEST(Logging, EnvInheritsSnapshot)
{
  Capture cap;
  logging::Scope _l = logging::Env{}.module("global");

  constexpr int kThreads = 16;

  std::vector<std::thread> pool;
  for (int i = 0; i < kThreads; ++i)
  {
    pool.emplace_back([snap = logging::snapshot()]
    {
      logging::Scope _l = snap;
      Log(LL::Info, "from thread");
    });
  }
  for (std::thread& t : pool) t.join();
  for (int i = 0; i < kThreads; i++) cap.expect("[global] from thread");
}
