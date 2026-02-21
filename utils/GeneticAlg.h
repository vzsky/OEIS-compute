#pragma once

#include <functional>
#include <random>

namespace genetic
{

struct SearchContext;

template <typename T>
concept IsGene = requires(T g, T h, const SearchContext& ctx) {
  { T::mutate(g, ctx) } -> std::convertible_to<T>;
  { T::crossover(g, h, ctx) } -> std::convertible_to<T>;
  { g.get_fitness() }; // must have operator + and comparable
};

struct SearchContext
{
  std::mt19937& rng;
  size_t generation;
};

template <typename Gene_>
  requires IsGene<Gene_>
class GeneticSearcher
{
  using Gene = Gene_;

private:
  std::mt19937 mRng{std::random_device{}()};
  std::uniform_real_distribution<double> mDie{0.0, 1.0};
  std::function<void(const Gene&, int)> mGenerationCallback = [](auto, auto) {};

  class RouletteSelector;

  SearchContext mCtx{mRng};

public:
  struct Config
  {
    double crossover_rate    = 0.3;
    uint64_t elite_count     = 0;
    uint64_t population_size = 10;
  } config;

  void setGenerationCB(std::function<void(const Gene&, int)> f) { mGenerationCallback = f; }

public:
  std::vector<Gene> search(const std::vector<Gene>& adams, int generations = 50)
  {
    std::vector<Gene> population = adams;

    size_t& g = mCtx.generation;
    for (g = 1; g <= generations; g++)
    {
      std::vector<Gene> new_population;
      auto rouletteSelector = RouletteSelector{population};

      while (new_population.size() < config.population_size)
      {
        int p1_idx = rouletteSelector.roll(mRng);
        Gene child = [&]
        {
          if (mDie(mRng) < config.crossover_rate)
          {
            int p2_idx = rouletteSelector.roll(mRng);
            return Gene::crossover(population[p1_idx], population[p2_idx], mCtx);
          }
          else
            return population[p1_idx];
        }();

        new_population.push_back(Gene::mutate(child, mCtx));
      }

      for (int i = 0; i < config.elite_count && i < population.size(); ++i)
        new_population.push_back(population[i]);

      population = std::move(new_population);
      std::ranges::sort(population, std::greater{}, &Gene::get_fitness);
      mGenerationCallback(population.front(), g);
    }

    return population;
  }
};

template <typename Gene>
  requires IsGene<Gene>
class GeneticSearcher<Gene>::RouletteSelector
{
private:
  mutable std::uniform_real_distribution<double> mDist;
  std::vector<double> mCumulativeFitness;

public:
  RouletteSelector(std::vector<Gene>& population)
  {
    mCumulativeFitness.reserve(population.size());
    double sum = 0.0;
    for (auto& g : population) sum += g.get_fitness(), mCumulativeFitness.push_back(sum);
    mDist = std::uniform_real_distribution(0.0, mCumulativeFitness.back());
  }

  int roll(std::mt19937& rng) const
  {
    double r = mDist(rng);
    auto it  = std::lower_bound(mCumulativeFitness.begin(), mCumulativeFitness.end(), r);
    return std::distance(mCumulativeFitness.begin(), it);
  }
};

} // namespace genetic
