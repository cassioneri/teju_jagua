#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"

#include <gtest/gtest.h>
#include <nanobench.h>

#include <algorithm>
#include <charconv>
#include <fstream>
#include <string>

namespace {

using namespace amaru;
using namespace ankerl::nanobench;

auto constexpr str_algorithm = "algorithm";
auto const     str_amaru     = std::string{"amaru"};
auto const     str_dragonbox = std::string{"dragonbox"};

auto constexpr str_binary    = "binary";
auto constexpr str_decimal   = "decimal";
auto constexpr str_csv       = R"DELIM("algorithm";"binary";"decimal";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algorithm)}};{{context(binary)}};{{context(decimal)}};{{minimum(elapsed)}};{{medianAbsolutePercentError(minimum)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
{{/result}})DELIM";

auto const to_chars_failure = amaru::exception_t{"to_chars failed."};

template <typename TInt>
char*
integer_to_chars(char* first, char* last, TInt const& value) {

  auto result = std::to_chars(first, last, value);

  if (result.ec == std::errc{})
    return const_cast<char*>(result.ptr);

  throw to_chars_failure;
}

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

template <typename T>
void benchmark(Bench& bench, T const value) {

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
      doNotOptimizeAway(traits_t::amaru(value));
  });

  bench
    .context(str_algorithm, str_dragonbox.c_str()   )
    .context(str_binary   , std::data(binary_chars) )
    .context(str_decimal  , std::data(decimal_chars))
    .run("", [&]() {
      doNotOptimizeAway(traits_t::dragonbox_full(value));
  });
}

void print_summary(Bench& bench) {

  auto results   = bench.results();
  auto summary   = Bench().unit(bench.unit());
  auto amaru     = Result{summary.config()};
  auto dragonbox = Result{summary.config()};

  for (auto const& result : results) {

    using namespace std::chrono;
    using seconds_t = duration<double, seconds::period>;

    auto const elapsed = duration_cast<nanoseconds>(
      seconds_t{result.minimum(Result::Measure::elapsed)});

    if (result.context(str_algorithm) == str_amaru)
      amaru.add(elapsed, 1, {});
    else
      dragonbox.add(elapsed, 1, {});
  }

  auto round_nano = [](double const x) {
    return static_cast<std::uint64_t>(1e12 * x + 0.5) / 1e12;
  };

  auto round_unit = [](double const x) {
    return static_cast<std::uint64_t>(1e3 * x + 0.5) / 1e3;
  };

  auto constexpr measure        = Result::Measure::elapsed;
  auto const     amaru_mean     = amaru.average(measure);
  auto const     dragonbox_mean = dragonbox.average(measure);
  auto const     baseline       = amaru_mean;

  std::cout <<
    "amaru     (mean)  = " << round_nano(amaru_mean) << '\n' <<
    //"          (MdAPE) = " << amaru.medianAbsolutePercentError(measure) <<
    //'\n' <<
    "          (rel.)  = " << round_unit(amaru_mean / baseline) <<
    "\n"
    "dragonbox (mean)  = " << round_nano(dragonbox_mean) << '\n' <<
    //"          (MdAPE) = " << dragonbox.medianAbsolutePercentError(measure) <<
    //'\n' <<
    "          (rel.)  = " << round_unit(dragonbox_mean / baseline) << '\n';
}

template <typename T>
void
benchmark_small_integers(const char* filename) {

  auto           out   = std::ofstream{filename};
  auto           bench = Bench().unit("number").output(nullptr);
  auto constexpr min   = T{1};
  auto constexpr max   = T{1000};

  for (T value = min; value < max; ++value) {
    benchmark(bench, value);
    ASSERT_NE(value + T{1}, value);
  }

  render(str_csv, bench, out);
  out.close();
  print_summary(bench);
}

TEST(float, small_integers) {
  benchmark_small_integers<float>("float_small_integers.csv");
}
 
TEST(double, small_integers) {
  benchmark_small_integers<double>("double_small_integers.csv");
}

} // namespace <anonymous>
