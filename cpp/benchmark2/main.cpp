#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "cpp/common/traits.hpp"

#include <gtest/gtest.h>
#include <nanobench.h>

namespace {

using namespace amaru;
using namespace ankerl::nanobench;

auto constexpr str_algo              = "algo";
auto constexpr str_amaru             = "amaru";
auto constexpr str_dragonbox_compact = "dragonbox (compact)";
auto constexpr str_dragonbox_full    = "dragonbox (full)";
auto constexpr str_value             = "value";
auto constexpr str_csv               = R"DELIM("algo";"value";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algo)}};{{context(value)}};{{median(elapsed)}};{{medianAbsolutePercentError(elapsed)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
{{/result}})DELIM";

template <typename T>
void benchmark(Bench& bench, T const value) {

  bench.relative(true)
    .context(str_algo, str_amaru)
    .context(str_value, "1.0").run("", [&]() {
      doNotOptimizeAway(traits_t<float>::amaru(value));
  });

  bench
    .context(str_algo, str_dragonbox_compact)
    .context(str_value, "1.0").run("", [&]() {
      doNotOptimizeAway(traits_t<float>::dragonbox_full(value));
  });

  bench
    .context(str_algo, str_dragonbox_full)
    .context(str_value, "1.0").run("", [&]() {
      doNotOptimizeAway(traits_t<float>::dragonbox_full(value));
  });

  render(str_csv, bench, std::cout);
  Result r;
}

TEST(float, small_integers) {

  auto bench = Bench().unit("number"); // .output(nullptr);

  benchmark(bench, 1.0f);

  //using fp_t = float;

  //// Let [1, M<T>[ be the interval of T-values for which the optimisation for
  //// small integers applies. The tested range is [1, min(M<T>, M<float>)[.

  //auto constexpr min = fp_t{1};
  //auto constexpr exp = std::min(traits_t<fp_t>::mantissa_size,
  //  traits_t<float>::mantissa_size);
  //auto const     max = std::pow(fp_t{2.0}, exp + 1);

  //for (fp_t value = min; value < max && !this->HasFailure(); ++value) {
  //  compare_to_other(value);
  //  ASSERT_NE(value + 1.0, value);
  //}
}

} // namespace <anonymous>
