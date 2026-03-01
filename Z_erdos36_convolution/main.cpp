#include "lib.h"
#include <math/Stats.h>
#include <random>
#include <utils/GeneticAlg.h>
#include <utils/Utils.h>

namespace
{

using namespace step_func;

template <typename T, std::size_t N> std::vector<double> erdos_convolute(const PiecewiseStepView<T, N>& f)
{
  assert(feq(f.area(), 0.5));
  for (int i = 0; i < N; i++)
    if (fgt(f[i], 1)) return {};

  auto g = one_minus(f);
  return convolute(f, g);
}

template <typename T, std::size_t N> double erdos_score(const PiecewiseStepView<T, N>& f)
{
  auto c = erdos_convolute(f);
  if (c.size() == 0) return 1;
  return 2 * std::ranges::max(c);
}

//===========================================================================================================
//= Gene: Symmetric Search
//=== Minimizing the score (max convoluted) assuming that the step function is symmetric
//===========================================================================================================

struct GeneConfig
{
  double variance_mult = 1;
};

template <int N, GeneConfig Cfg> class SymmetricGene
{

protected:
  PiecewiseStep<N> mSteps;
  double mScore   = 0;
  double mFitness = 0;

  mutable std::normal_distribution<double> mNudge{0.0, 0.05};
  mutable std::uniform_real_distribution<double> mDice{0.0, 1.0};

  static std::uniform_int_distribution<int> distN;

public:
  SymmetricGene(PiecewiseStep<N> f) : mSteps{f} { _calculate_score(); }

public:
  static SymmetricGene mutate(const SymmetricGene& g, const genetic::SearchContext& ctx)
  {
    auto heights = g.mSteps.get_heights();

    for (double& h : heights)
    {
      double rolled = g.mDice(ctx.rng);
      if (rolled < 0.2)
      {
        h = std::max(0.0, h + g.mNudge(ctx.rng));
      }
      else if (rolled < 0.3)
      {
        h = g.mDice(ctx.rng);
      }
    }
    return {heights};
  }

  static SymmetricGene crossover(const SymmetricGene& mom, const SymmetricGene& dad,
                                 const genetic::SearchContext& ctx)
  {
    auto h1 = mom.mSteps.get_heights();
    auto h2 = dad.mSteps.get_heights();
    std::array<double, N> child_h;

    int cut = distN(ctx.rng);
    for (int i = 0; i < N; ++i)
    {
      child_h[i] = (i < cut) ? h1[i] : h2[i];
    }
    return {child_h};
  }

  double get_fitness() const { return mFitness; }
  double get_score() const { return mScore; }
  PiecewiseStep<N> get_function() const { return mSteps; }

protected:
  void _calculate_score()
  {
    auto c = erdos_convolute(normalize(mirror(mSteps)));
    if (c.size() == 0)
    {
      mScore   = 1.0; // high score = low fitness
      mFitness = 1.0;
    }
    else
    {
      mScore   = 2 * std::ranges::max(c);
      auto f   = 2 * std::ranges::max(c) - Cfg.variance_mult * stats::variance(c);
      mFitness = 1 / f;
    }
  }
};

template <int N, GeneConfig Cfg> std::uniform_int_distribution<int> SymmetricGene<N, Cfg>::distN{0, N};

static_assert(genetic::IsGene<SymmetricGene<0, {}>>);

template <GeneConfig C2, int N, GeneConfig C1>
std::vector<SymmetricGene<N, C2>> migrate_population(const std::vector<SymmetricGene<N, C1>>& p)
{
  std::vector<SymmetricGene<N, C2>> new_p;
  for (const SymmetricGene<N, C1>& gene : p) new_p.emplace_back(gene.get_function());
  return new_p;
}

//===========================================================================================================

// clang-format off
PiecewiseStep<8> Haugland15 ({ 0.0, 0.09938602, 0.64299877, 0.36104582, 0.69536426, 0.59241335, 0.89573331, 0.92611694 });
PiecewiseStep<26> Haugland51 ({ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0002938681556273, 0.5952882223921177, 0.7844530825484313, 0.8950034338013842, 0.0597964076006748, 0.0189602838469592, 0.7420501628172980, 0.6444559588500921, 0.3549040817844764, 0.8762442385073478, 0.5437907313675501, 0.2679640048997296, 0.8518954615823791, 0.5211171156914872, 1.0, 0.5506146790047043, 0.9007715390796991, 0.8229000691941086, 0.8879541710440111, 0.9315424878319221, 1.0 });
// clang-format on

} // namespace

int main()
{
  // [Haugland, 1996] showed we need to find a PiecewiseStep from [0, 2] with f(x) <= 1 and
  // area = 1 the minimizes the max(convolute(f, 1-f));
  // this is equivalent to function in [0, 1] with area 0.5

  using namespace step_func;

  utils::timeit([]
  {
    std::cout << erdos_score(mirror(Haugland51)) << std::endl;
    assert(feq(erdos_score(mirror(Haugland51)), 0.380927));
    for (int i = 0; i < 1000; i++)
    {
      erdos_score(mirror(Haugland51));
    }
  });

  utils::timeit([]
  {
    constexpr int K       = 1;
    constexpr int M       = 9;
    constexpr int N       = M * K;
    PiecewiseStep<K> one  = (std::array<double, 1>{1});
    PiecewiseStep<N> ones = expand<M>(one);

    const auto print_score = [](const auto& g, int gen)
    {
      if (gen % 100 == 0) std::cout << "gen: " << gen << " = " << g.get_score() << std::endl;
    };

    auto era1_population = [&]
    {
      constexpr GeneConfig Config{.variance_mult = 20};
      using Gene = SymmetricGene<N, Config>;
      Gene adam  = ones;

      genetic::GeneticSearcher<Gene> g;
      g.config.elite_count     = 1000;
      g.config.population_size = 5000;
      g.setGenerationCB(print_score);

      return g.search({adam}, 1000);
    }();

    auto era2_population = [&]
    {
      constexpr GeneConfig Config{.variance_mult = 10};
      using Gene = SymmetricGene<N, Config>;

      genetic::GeneticSearcher<Gene> g;
      g.config.elite_count     = 1000;
      g.config.population_size = 5000;
      g.setGenerationCB(print_score);

      return g.search(migrate_population<Config>(era1_population), 1000);
    }();

    auto era3_population = [&]
    {
      constexpr GeneConfig Config{.variance_mult = 0};
      using Gene = SymmetricGene<N, Config>;

      genetic::GeneticSearcher<Gene> g;
      g.config.elite_count     = 1000;
      g.config.population_size = 5000;
      g.setGenerationCB(print_score);

      return g.search(migrate_population<Config>(era2_population), 1000);
    }();

    utils::print_range(era3_population.front().get_function().get_heights(), ", ");
  });
}
