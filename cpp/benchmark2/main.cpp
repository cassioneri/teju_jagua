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
#include <random>
#include <string>

namespace {

auto constexpr str_algorithm = "algorithm";
auto const     str_amaru     = std::string{"amaru"};
auto const     str_dragonbox = std::string{"dragonbox"};

auto constexpr str_binary    = "binary";
auto constexpr str_decimal   = "decimal";
auto constexpr str_csv       = R"DELIM("algorithm";"binary";"decimal";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algorithm)}};{{context(binary)}};{{context(decimal)}};{{average(elapsed)}};{{medianAbsolutePercentError(elapsed)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
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
fields_to_chars(char* first, char* last, TFields const& fields, int const base)
{
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

auto get_bench() {
  return nanobench::Bench()
    .unit("number")
    .clockResolutionMultiple(1000)
    .minEpochIterations(16)
    .epochs(5)
    .output(nullptr);
}

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
void output(nanobench::Bench const& bench, const char* const filename) {

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

    #define AMARU_FIELD(a) std::setw(6) << a

    std::cout << std::setprecision(3) << std::fixed <<
      "\n"
      "amaru     (mean  ) = " << AMARU_FIELD(scale * amaru_mean        ) << " ns\n"
      "          (stddev) = " << AMARU_FIELD(scale * amaru.stddev()    ) << " ns\n"
      "          (rel.  ) = " << AMARU_FIELD(amaru_mean / baseline     ) << "\n"
      "\n"
      "dragonbox (mean  ) = " << AMARU_FIELD(scale * dragonbox_mean    ) << " ns\n"
      "          (stddev) = " << AMARU_FIELD(scale * dragonbox.stddev()) << " ns\n"
      "          (rel.  ) = " << AMARU_FIELD(dragonbox_mean / baseline ) << "\n"
      "\n";

      #undef AMARU_FIELD
  }
}

// TODO (CN) Document.
template <typename T>
void
benchmark_small_integers(const char* const filename) {

  auto           bench = get_bench();
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

template <typename T>
void
benchmark_centred(const char* const filename, unsigned n_mantissas) {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  auto bench = get_bench();

  auto constexpr mantissa_max = amaru_pow2(u1_t, traits_t::mantissa_size) - 1;
  auto           distribution = std::uniform_int_distribution<u1_t>
    {1, mantissa_max};
  std::mt19937_64 device;

  auto constexpr exponent_max = amaru_pow2(std::int32_t,
    traits_t::exponent_size) - 1;

  while (n_mantissas--) {

    auto const mantissa = distribution(device);

    if (mantissa == 0)
      continue;

    for (std::int32_t exponent = 0; exponent < exponent_max; ++exponent) {
      auto const ieee  = typename traits_t::fields_t{ exponent, mantissa };
      auto const value = traits_t::ieee_to_value(ieee);
      benchmark(bench, value);
    }
  }

  output(bench, filename);
}

TEST(float, centred) {
  benchmark_centred<float>("float_centred.csv", 10);
}

TEST(double, centred) {
  benchmark_centred<double>("double_centred.csv", 10);
}

} // namespace <anonymous>
