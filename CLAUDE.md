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
- `maya/StringFormat.hpp` — `maya::FmtT<S>` (format string wrapper); `"lit"_f` (UDL, distinct from `_ms`); `IsMayaFormatT` concept. Only `_f` strings satisfy `IsMayaFormatT` — `_ms` strings do not.
- `maya/Tagged.hpp` — `maya::Tagged<Tag, T>` strongly-typed value wrapper; implicit conversions to/from `T` in context; not the same as an explicit cast
- `maya/Dictionary.hpp` — `maya::Dict<Entries...>`, compile-time key-value store

Do not use anything inside any `::detail` namespace — those are implementation namespaces.

**Shared libraries** live in `utils/` and are compiled as individual static libraries:
- `utils` / `Utils.hpp` — `utils::ScopeTimer` (re-export), `read_bfile`; also pulls in `Parallel.hpp` and `Timer.hpp`
- `prime` / `Prime.hpp` — `Prime<N>` sieve; free functions `is_prime`, `is_coprime`
- `primeint` / `PrimeInt.hpp` — prime-factorization integer type (depends on `prime`)
- `bigint` / `BigInt.hpp` — GMP-backed arbitrary precision integers (`libgmp`, `libgmpxx`); also `#include`s `SlowBigInt.tpp` which provides `slow_bigint::DecBigInt`, `DenseDecBigInt`, `DenseBigInt` (software bigint, base-parameterized)
- `fraction` / `Fraction.hpp` — `Fraction<Int>` rational arithmetic (depends on `bigint`)
- `modint` / `ModInt.hpp` — `ModInt<Mods...>` CRT residue vector; division enabled only when all Mods are prime
- `fft` / `Fft.hpp` — `fft::convolution`, `fft::self_convolution`, `fft::transform<Direction>`
- `treap` / `Treap.hpp` — `Treap<Value, Compare>` with parallel `gather`/`fmap`
- `complementnonnvector` / `ComplementNonnVector.hpp` — complement of non-negative vector
- `parallel` / `Parallel.hpp` — `utils::parallel::foreach`, `all_of`, `filter`; all accept `MonitorConfig` template arg for progress logging
- `timer` / `Timer.hpp` — `utils::timer::ScopeTimer` RAII timer (re-exported as `utils::ScopeTimer` via `Utils.hpp`)
- `hash` / `Hash.hpp` — `hash::combine(vs...)`, `hash::Hasher<T>` (calls `v.hash()`)
- `format` / `Format.hpp` — `format::format(fmt, args...)` using `_f` format strings
- `metaprog` / `MetaProg.hpp` — `mp::For<I, N>(f)` compile-time integer loop
- `geneticalg` / `GeneticAlg.hpp` — `genetic::GeneticSearcher<Gene>` roulette-selection GA
- `allutils` — interface target that links all of the above

**Math headers** in `math/`:
- `math/Basic.hpp` — `math::pow`, `math::gcd`, `math::lcm`, `math::fact`, `math::nCk`
- `math/Stats.hpp` — statistics utilities
- `math/numtheory/Jacobi.hpp` — Jacobi symbol

**Logging** (from `utils/Logging.hpp`, included transitively via `Utils.hpp`):
- `Log(lvl, ...)` — variadic print with optional module prefix; `lvl` is `LL::Infra`/`LL::Debug`/`LL::Info`/`LL::Warn`/`LL::Error` (lowest to highest)
- `Log(lvl, "fmt $"_f, ...)` — `$`-placeholder format string when first arg after level is a maya format string (`_f` UDL); placeholder count is checked at compile time. Use `_f`, not `_ms` — `_ms` strings do not satisfy `IsMayaFormatT`.
- `LL::Infra` — lowest level; use for construction/initialization noise (e.g. "constructing Prime<N>", "reading file"). Below `Debug` in severity.
- `LL` — shorthand for `logging::detail::LogLevel`
- `"name"_ms` — compile-time string value; exported from `Logging.hpp`

The active environment (module label, level threshold, output sink) is a **runtime,
thread-local stack** read at log time, so it propagates into functions you call.
Override it for the current scope (and everything it calls) with an RAII `logging::Scope`
built from a fluent spec; the previous env is restored at scope exit:
```cpp
logging::Scope _l = logging::Env{}.module("server");              // append module label
logging::Scope _l = logging::Env{}.level(LL::Warn);               // raise threshold
logging::Scope _l = logging::Env{}.module("db").level(LL::Debug); // both at once
logging::Scope _l = logging::Env{}.logger(loggers::progress);     // swap sink (normal/timestamp/progress/noop)
```

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

Use `Log` instead of `std::cout` or `std::cerr` for all output.

`maya::Tagged<T, tag>` carries implicit conversion operators to `T`, so let the context do the work — do not add `static_cast<T>` or other explicit casts just to extract the underlying value when an implicit conversion suffices (e.g. arithmetic, comparisons, function arguments).

Scope-guard conventions: name a `logging::Scope` `_l` and a `utils::ScopeTimer` `_t`, and declare it
as the **first statement** of its enclosing scope. If a scope has both, the timer (`_t`) comes first,
then the log scope (`_l`).

Mark all pure functions (those that return a value whose discard is likely a bug) with `[[nodiscard]]`. This includes: all `const` member functions returning by value or reference, all free functions that compute and return a result, arithmetic/comparison operators, accessor getters, and factory functions. Compound-assignment operators (`+=`, `/=`, …) and `void`-returning functions are exempt.

### Artifacts / CI

GitHub Actions generates baseline benchmark artifacts on each commit. 
To investigate a regression: download the artifact files into `/artifacts`, run `./scripts/compare_artifact.sh`,
then optionally commit new baselines with `./scripts/use_artifacts.sh`.
