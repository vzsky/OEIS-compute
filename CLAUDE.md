# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

The primary entrypoint is the `./run` script:

```bash
./run <directory>          # build and run a sequence program (e.g. ./run A061955)
./run test <module>        # build and run tests for a module (e.g. ./run test prime)
./run bench <module>       # build and run benchmarks with regression check (e.g. ./run bench prime)
./run profile <directory>  # build and run with gperftools CPU profiler
./run all                  # build everything
./run cmake                # (re-)configure CMake
```

Build output goes to `./build/bin/`. The build directory is created automatically on first run via `cmake --preset=default`
(uses Ninja, outputs to `./build`).

To run all tests: `./run tests`

To format code: `clang-format` — the project has a `.clang-format` config (2-space indent, 110 column limit, Allman brace style).

## Architecture

### Repository structure

Each OEIS sequence gets its own directory (e.g. `A061955/`, `A002326/`) containing a `main.cpp` and `CMakeLists.txt`. 
The root `CMakeLists.txt` auto-discovers and adds all subdirectories that contain a `CMakeLists.txt`.

**`utils/maya/`** — compile-time-only headers (no runtime presence; "maya" = illusion). Header-only, nothing to link:
- `maya/String.hpp` — `maya::StrT<"lit">` (compile-time string type); `"lit"_ms` (UDL); `IsMayaStr` concept
- `maya/Dictionary.hpp` — `maya::Dict<Entries...>`, compile-time key-value store

Do not use anything inside any `::detail` namespace — those are implementation namespaces.

**Shared libraries** live in `utils/` and are compiled as individual static libraries:
- `utils` / `Utils.hpp` — `par_all_of`, `ScopeTimer`, `read_bfile` - utils functions 
- `prime` / `Prime.hpp` — prime sieve/testing
- `primeint` — prime-backed integer type (depends on `prime`)
- `bigint` / `BigInt.hpp` — arbitrary precision integers backed by GMP (`libgmp`, `libgmpxx`)
- `fraction` / `Fraction.hpp` — rational arithmetic (depends on `bigint`)
- `modint` / `ModInt.hpp` — modular arithmetic
- `fft` / `Fft.hpp` — FFT
- `treap` / `Treap.hpp` — treap data structure
- `complementnonnvector` / `ComplementNonnVector.hpp` — complement of non-negative vector
- `allutils` — interface target that links all of the above

**Math headers** in `math/`:
- `math/Basic.hpp` — `math::pow`, `math::gcd`, `math::lcm`, `math::fact`, `math::nCk`
- `math/Stats.hpp` — statistics utilities
- `math/numtheory/Jacobi.hpp` — Jacobi symbol

**Logging** (from `utils/Logging.hpp`, included transitively via `Utils.hpp`):
- `Log(lvl, ...)` — variadic print with optional module prefix; `lvl` is `Debug`/`Info`/`Warn`/`Error`
- `LogF(lvl, fmt, ...)` — `$`-placeholder format string, compile-time placeholder count check
- `_LogEnv_` — compile-time type alias tracking current module name and log level; default is empty name + `Info`
- `LL` — shorthand for `logging::details::LogLevel`
- `"name"_ms` — compile-time string value; exported from `Logging.hpp`

Set module name or level inline with `using _LogEnv_ = _LogEnv_::...` — operations can be chained:
```cpp
using _LogEnv_ = _LogEnv_::Module<"server">;           // append module label (separator " | " is baked in)
using _LogEnv_ = _LogEnv_::Level<LL::Warn>;            // raise threshold
using _LogEnv_ = _LogEnv_::Module<"db">::Level<LL::Debug>; // both at once
```
Nesting blocks is how scoped restore works — inner `using _LogEnv_` shadows outer; exiting the block restores it.

### Adding a new sequence

Copy `template/` to a new directory named after the sequence (e.g. `A123456/`). 
The template `CMakeLists.txt` uses `get_filename_component` to derive the executable name from the directory nameand links against `allutils`.
We don't really care about compile time so no need to opt out of `allutils`

### Tests

Tests live in `tests/` and use GoogleTest (fetched via CMake `FetchContent`). 
Each test module is named `<module>_test` and registered with CTest. 
Tests are run with `--gtest_shuffle`.

### Benchmarks

Benchmarks live in `benchmark/` and use Google Benchmark (fetched via CMake `FetchContent`). 
Each benchmark target is named `<name>_bench`. 
Running via `./run bench <name>` automatically compares against local baselines in `benchmark/local_baseline/` using `scripts/regression_test.py`.

## Coding style

Prefer modern C++ over C idioms:
- Lambdas over function pointers
- `static_cast<T>` over C-style casts
- `std::array` / `std::span` over raw arrays
- Range-based `for` and standard algorithms over raw index loops where natural
- `nullptr` over `NULL`

Private member variables use `mVar` naming (camelCase with `m` prefix), not `var_`.

Include order: standard library headers with `<angle brackets>`, then project headers also with `<angle brackets>` using their full path (e.g. `<utils/Logging.hpp>`), never with `"quotes"`.

Use `Log`/`LogF` instead of `std::cout` or `std::cerr` for all output.

### Artifacts / CI

GitHub Actions generates baseline benchmark artifacts on each commit. 
To investigate a regression: download the artifact files into `/artifacts`, run `./scripts/compare_artifact.sh`,
then optionally commit new baselines with `./scripts/use_artifacts.sh`.
