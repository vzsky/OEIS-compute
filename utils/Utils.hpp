#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <source_location>
#include <stdexcept>
#include <vector>

#include <utils/Logging.hpp>
#include <utils/Parallel.hpp>
#include <utils/Timer.hpp>

namespace utils
{

using timer::ScopeTimer;

template <typename T>
[[nodiscard]] inline std::vector<T>
read_bfile(const std::string& relative_path,
           const std::source_location& loc = std::source_location::current())
{
  namespace fs = std::filesystem;

  fs::path base = fs::path(loc.file_name()).parent_path();
  fs::path path = base / relative_path;

  std::ifstream in(path);
  if (!in) throw std::runtime_error("cannot open file");

  std::vector<T> out;

  std::string line;
  while (std::getline(in, line))
  {
    if (line.empty() || line[0] == '#') continue;

    std::istringstream iss(line);

    size_t n, a; // index and number, be careful of reading to int8_t (char)
    if (iss >> n >> a) out.push_back(a);
  }

  return out;
}

} // namespace utils
