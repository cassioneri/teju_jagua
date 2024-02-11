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

namespace {

using namespace amaru;
using namespace ankerl::nanobench;

auto constexpr str_algorithm = "algorithm";
auto constexpr str_amaru     = "amaru";
auto constexpr str_dragonbox = "dragonbox";
auto constexpr str_binary    = "binary";
auto constexpr str_decimal   = "decimal";
auto constexpr str_csv       = R"DELIM("algorithm";"binary";"decimal";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algorithm)}};{{context(binary)}};{{context(decimal)}};{{median(elapsed)}};{{medianAbsolutePercentError(elapsed)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
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
void benchmark(Bench& bench, T const value, std::ostream& out) {

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
    .context(str_algorithm, str_amaru               )
    .context(str_binary   , std::data(binary_chars) )
    .context(str_decimal  , std::data(decimal_chars))
    .run(str_amaru, [&]() {
      doNotOptimizeAway(traits_t::amaru(value));
  });

  bench
    .context(str_algorithm, str_dragonbox           )
    .context(str_binary   , std::data(binary_chars) )
    .context(str_decimal  , std::data(decimal_chars))
    .run(str_dragonbox, [&]() {
      doNotOptimizeAway(traits_t::dragonbox_full(value));
  });

  render(str_csv, bench, out);
}

template <typename T>
void
benchmark_small_integers(const char* filename) {

  auto out   = std::ofstream{ filename };
  auto bench = Bench().unit("number").output(nullptr);

  auto constexpr min = T{1};
  auto constexpr max = T{3};

  for (T value = min; value < max; ++value) {
    benchmark(bench, value, out);
    ASSERT_NE(value + T{1}, value);
  }
}

TEST(float, small_integers) {
  benchmark_small_integers<float>("float_small_integers.csv");
}

TEST(double, small_integers) {
  benchmark_small_integers<double>("double_small_integers.csv");
}

} // namespace <anonymous>
