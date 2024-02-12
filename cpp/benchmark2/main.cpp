#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "cpp/benchmark2/stats.hpp"
#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"

#include <gtest/gtest.h>
#include <nanobench.h>

#include <algorithm>
#include <charconv>
#include <fstream>
#include <string>

namespace {

auto constexpr str_algorithm = "algorithm";
auto const     str_amaru     = std::string{"amaru"};
auto const     str_dragonbox = std::string{"dragonbox"};

auto constexpr str_binary    = "binary";
auto constexpr str_decimal   = "decimal";
auto constexpr str_csv       = R"DELIM("algorithm";"binary";"decimal";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algorithm)}};{{context(binary)}};{{context(decimal)}};{{minimum(elapsed)}};{{medianAbsolutePercentError(minimum)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
{{/result}})DELIM";

auto const to_chars_failure = amaru::exception_t{"to_chars failed."};

// TODO (CN) Document.
template <typename TInt>
char*
integer_to_chars(char* first, char* last, TInt const& value) {

  auto result = std::to_chars(first, last, value);

  if (result.ec == std::errc{})
    return const_cast<char*>(result.ptr);

  throw to_chars_failure;
}

// TODO (CN) Document.
template <typename TFields>
char*
fields_to_chars(char* first, char* last, TFields const& fields, int base) {

  auto       ptr  = integer_to_chars(first, last, fields.c.mantissa);
  auto const size = last - ptr;

  if (base == 2 && size > 3)
    ptr = std::copy_n("*2^", 3, ptr);
  else if (base == 10 && size > 4)
    ptr = std::copy_n("*10^", 4, ptr);
  else
    throw to_chars_failure;

  return integer_to_chars(ptr, last, fields.c.exponent);
}

namespace nanobench = ankerl::nanobench;

// TODO (CN) Document.
template <typename T>
void benchmark(nanobench::Bench& bench, T const value) {

  using      traits_t = amaru::traits_t<T>;
  using      buffer_t = char[40];

  auto const ieee    = traits_t::value_to_ieee(value);
  auto const binary  = traits_t::ieee_to_amaru_binary(ieee);
  auto const decimal = traits_t::amaru(value);

  buffer_t binary_chars{};
  fields_to_chars(std::begin(binary_chars), std::prev(std::end(binary_chars)),
    binary, 2);

  buffer_t decimal_chars{};
  fields_to_chars(std::begin(decimal_chars), std::prev(std::end(decimal_chars)),
    decimal, 10);

  bench.relative(true)
    .context(str_algorithm, str_amaru.c_str()       )
    .context(str_binary   , std::data(binary_chars) )
    .context(str_decimal  , std::data(decimal_chars))
    .run("", [&]() {
      nanobench::doNotOptimizeAway(traits_t::amaru(value));
  });

  bench
    .context(str_algorithm, str_dragonbox.c_str()   )
    .context(str_binary   , std::data(binary_chars) )
    .context(str_decimal  , std::data(decimal_chars))
    .run("", [&]() {
      nanobench::doNotOptimizeAway(traits_t::dragonbox_full(value));
  });
}

// TODO (CN) Document.
void output(nanobench::Bench const& bench, const char* filename) {

  // Save detailed results in csv file.
  {
    auto out = std::ofstream{filename};
    render(str_csv, bench, out);
  }

  // Print summary to std::cout.
  {
    auto results   = bench.results();
    auto amaru     = amaru::stats_t{};
    auto dragonbox = amaru::stats_t{};

    for (auto const& result : results) {

      using picoseconds_t = std::chrono::duration<double, std::pico>;
      using seconds_t     = std::chrono::duration<double, std::ratio<1>>;

      auto const measure  = nanobench::Result::Measure::elapsed;
      auto const elapsed  = seconds_t{result.minimum(measure)};
      auto const value    = std::uint64_t(picoseconds_t{elapsed}.count());

      if (result.context(str_algorithm) == str_amaru)
        amaru.update(value);
      else
        dragonbox.update(value);
    }

    auto const     amaru_mean     = amaru.mean();
    auto const     dragonbox_mean = dragonbox.mean();
    auto const     baseline       = double(amaru_mean);
    auto constexpr scale          = 0.001;

    std::cout << std::setprecision(3) << std::fixed <<
      "(Time in nanoseconds)\n"
      "\n"
      "amaru     (mean  ) = " << scale * amaru_mean         << "\n"
      "          (stddev) = " << scale * amaru.stddev()     << "\n"
      "          (rel.  ) = " << amaru_mean / baseline      << "\n"
      "dragonbox (mean  ) = " << scale * dragonbox_mean     << "\n"
      "          (stddev) = " << scale * dragonbox.stddev() << "\n"
      "          (rel.  ) = " << dragonbox_mean / baseline  << '\n';
  }
}

// TODO (CN) Document.
template <typename T>
void
benchmark_small_integers(const char* filename) {

  auto           bench = nanobench::Bench()
    .unit("number").output(nullptr);
  auto constexpr min   = T{1};
  auto constexpr max   = T{1000};

  for (T value = min; value < max; ++value) {
    benchmark(bench, value);
    ASSERT_NE(value + T{1}, value);
  }

  output(bench, filename);
}

TEST(float, small_integers) {
  benchmark_small_integers<float>("float_small_integers.csv");
}
 
TEST(double, small_integers) {
  benchmark_small_integers<double>("double_small_integers.csv");
}

} // namespace <anonymous>
