// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/benchmark/benchmark.cpp
 *
 * Benchmark Teju Jagua against other algorithms.
 */

#include "teju/common.h"
#include "teju/config.h"
#include "teju/double.h"
#include "teju/float.h"
#include "cpp/benchmark/stats.hpp"
#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"

#include <gtest/gtest.h>
#include <nanobench.h>

#include <algorithm>
#include <charconv>
#include <fstream>
#include <random>
#include <string>
#include <string_view>

#if defined(__unix__)
  #include <unistd.h>
#endif

namespace {

auto constexpr run_teju      = true;
auto constexpr run_dragonbox = true;
auto constexpr run_ryu       = true;

auto constexpr str_algorithm = "algorithm";
auto const     str_teju      = std::string_view{"teju"};
auto const     str_dragonbox = std::string_view{"dragonbox"};
auto const     str_ryu       = std::string_view{"ryu"};

auto constexpr str_binary    = "binary";
auto constexpr str_decimal   = "decimal";

// https://nanobench.ankerl.com/reference.html#render-mustache-like-templates
auto constexpr str_csv       = std::string_view{
  R"DELIM("algorithm";"binary";"decimal";"elapsed";"error %";"instructions";"branches";"branch misses";"total"
{{#result}}{{context(algorithm)}};{{context(binary)}};{{context(decimal)}};{{average(elapsed)}};{{medianAbsolutePercentError(elapsed)}};{{median(instructions)}};{{median(branchinstructions)}};{{median(branchmisses)}};{{sumProduct(iterations, elapsed)}}
{{/result}})DELIM"};

auto const to_chars_failure = teju::exception_t{"to_chars failed."};

/**
 * @brief Converts an integer into chars.
 *
 * Simply delegates to std::to_chars and throws in case of failure. A buffer for
 * the chars must be provided by the caller.
 *
 * @tparam TInt             The type of integer to be converted.
 *
 * @param  begin            Pointer to beginning of the buffer.
 * @param  end              Pointer to one-past-the-end of the buffer.
 * @param  value            The integer to be converted.
 *
 * @returns Pointer to one-past-the-end of characters written.
 */
template <typename TInt>
char*
integer_to_chars(char* const begin, char* const end, TInt const& value) {

  auto result = std::to_chars(begin, end, value);

  if (result.ec == std::errc{})
    return const_cast<char*>(result.ptr);

  throw to_chars_failure;
}

/**
 * @brief Converts fields to chars.
 *
 * A buffer for the chars must be provided by the caller.
 *
 * @tparam TFields          The type of fields.
 *
 * @param  begin            Pointer to beginning of the buffer.
 * @param  end              Pointer to one-past-the-end of the buffer.
 * @param  fields           The fields to be converted.
 * @param  base             The base for the exponent, either 2 or 10.
 *
 * @returns Pointer to one-past-the-end of characters written.
 */
template <typename TFields>
char*
fields_to_chars(char* const begin, char* const end, TFields const& fields,
  int const base)
{
  auto       ptr  = integer_to_chars(begin, end, fields.mantissa);
  auto const size = end - ptr;

  if (base == 2 && size > 3)
    ptr = std::copy_n("*2^", 3, ptr);
  else if (base == 10 && size > 4)
    ptr = std::copy_n("*10^", 4, ptr);
  else
    throw to_chars_failure;

  return integer_to_chars(ptr, end, fields.exponent);
}

namespace nanobench = ankerl::nanobench;

/**
 * @brief Gets a configured nanobench::Bench object for the benchmark.
 *
 * @returns The Bench object.
 */
auto get_bench() {
  return nanobench::Bench()
    .unit("number")
    .epochs(5)
    .output(nullptr);
}

/**
 * @brief Benchmarks conversion of a given floating-point number to its decimal
 *        representation.
 *
 * @tparam T                The floating-point number type.
 *
 * @param  bench            The benchmark object recording the results.
 * @param  value            The given floating-point number.
 */
template <typename T>
void
benchmark(nanobench::Bench& bench, T const value) {

  using      traits_t = teju::traits_t<T>;
  using      buffer_t = char[40];

  auto const binary   = traits_t::to_binary(value);
  auto const decimal  = traits_t::teju(value);

  buffer_t binary_chars{};
  fields_to_chars(std::begin(binary_chars), std::prev(std::end(binary_chars)),
    binary, 2);

  buffer_t decimal_chars{};
  fields_to_chars(std::begin(decimal_chars), std::prev(std::end(decimal_chars)),
    decimal, 10);

  if constexpr (run_teju)
    bench.relative(true)
      .context(str_algorithm, str_teju.data()         )
      .context(str_binary   , std::data(binary_chars) )
      .context(str_decimal  , std::data(decimal_chars))
      .run("", [&]() {
        nanobench::doNotOptimizeAway(traits_t::teju_raw(value));
    });

  if constexpr (run_dragonbox)
    bench
      .context(str_algorithm, str_dragonbox.data()    )
      .context(str_binary   , std::data(binary_chars) )
      .context(str_decimal  , std::data(decimal_chars))
      .run("", [&]() {
        nanobench::doNotOptimizeAway(traits_t::dragonbox_raw(value));
    });

  if constexpr (run_ryu)
    bench
      .context(str_algorithm, str_ryu.data()          )
      .context(str_binary   , std::data(binary_chars) )
      .context(str_decimal  , std::data(decimal_chars))
      .run("", [&]() {
        nanobench::doNotOptimizeAway(traits_t::ryu_raw(value));
    });
}

/**
 * @brief Streams out detailed benchmarks results to a given file and a summary
 *        to std::cout.
 *
 * @param  bench            The benchmark object recording the results.
 * @param  filename         The name of the output file.
 */
void
output(nanobench::Bench const& bench, std::string_view const filename) {

  // Save detailed results in csv file.
  {
    auto out = std::ofstream{filename.data()};
    render(str_csv.data(), bench, out);
  }

  // Print summary to std::cout.
  {
    auto results    = bench.results();
    auto teju       = teju::stats_t{};
    auto dragonbox  = teju::stats_t{};
    auto ryu        = teju::stats_t{};

    for (auto const& result : results) {

      using picoseconds_t = std::chrono::duration<double, std::pico>;
      using seconds_t     = std::chrono::duration<double, std::ratio<1>>;

      auto const measure  = nanobench::Result::Measure::elapsed;
      auto const elapsed  = seconds_t{result.minimum(measure)};
      auto const value    = std::uint64_t(picoseconds_t{elapsed}.count());

      if (run_teju && result.context(str_algorithm) == str_teju)
        teju.update(value);
      else if (run_dragonbox && result.context(str_algorithm) == str_dragonbox)
        dragonbox.update(value);
      else if (run_ryu &&  result.context(str_algorithm) == str_ryu)
        ryu.update(value);
    }

    auto const     teju_mean      = run_teju      ? teju.mean()      : 1;
    auto const     dragonbox_mean = run_dragonbox ? dragonbox.mean() : 1;
    auto const     ryu_mean       = run_ryu       ? ryu.mean()       : 1;
    auto const     baseline       = double(teju_mean);
    auto constexpr scale          = 0.001;

    #define teju_field(a) std::setw(6) << a

    if constexpr (run_teju)
      std::cout << std::setprecision(3) << std::fixed <<
        "\n"
        "teju      (mean  ) = " << teju_field(scale * teju_mean         ) <<
        " ns\n"
        "          (stddev) = " << teju_field(scale * teju.stddev()     ) <<
        " ns\n"
        "          (rel.  ) = " << teju_field(teju_mean / baseline      ) <<
        "\n\n";

    if constexpr (run_dragonbox)
      std::cout << std::setprecision(3) << std::fixed <<
        "dragonbox (mean  ) = " << teju_field(scale * dragonbox_mean    ) <<
        " ns\n"
        "          (stddev) = " << teju_field(scale * dragonbox.stddev()) <<
        " ns\n"
        "          (rel.  ) = " << teju_field(dragonbox_mean / baseline ) <<
        "\n\n";

    if constexpr (run_ryu)
      std::cout << std::setprecision(3) << std::fixed <<
        "ryu       (mean  ) = " << teju_field(scale * ryu_mean          ) <<
        " ns\n"
        "          (stddev) = " << teju_field(scale * ryu.stddev()      ) <<
        " ns\n"
        "          (rel.  ) = " << teju_field(ryu_mean / baseline       ) <<
        "\n\n";

      #undef teju_field
  }
}

/**
 * @brief Benchmarks conversion of integral floating-point numbers to their
 *        decimal representations. Streams out detailed benchmarks results to a
 *        given file and a summary to std::cout.
 *
 * The set of values comprises the smallest and largest 5,000 integral values (a
 * maximum of 10,000 values) which Teju Jagua treats as special cases.
 *
 * @tparam T                The floating-point number type.
 *
 * @param  filename         The name of the output file.
 */
template <typename T>
void
benchmark_integers(std::string_view const filename) {

  auto bench = get_bench();

  auto test  = [&](T const min, T const max) {
    for (T value = min; value < max; ++value)
      benchmark(bench, value);
  };

  auto const max = std::pow(T{2}, T{teju::traits_t<T>::mantissa_size});

  test(T{1}, T{5000});
  test(max - T{5000}, max);

  output(bench, filename);
}

TEST(float, integers) {
  benchmark_integers<float>("float_integers.csv");
}

TEST(double, integers) {
  benchmark_integers<double>("double_integers.csv");
}

/**
 * @brief Benchmarks conversion of floating-point numbers with a given mantissa.
 *        Streams out detailed benchmarks results to a given file and a summary
 *        to std::cout.
 *
 * @tparam T                The floating-point number type.
 *
 * @param  bench            The benchmark object recording the results.
 * @param  filename         The name of the output file.
 * @param  mantissa         The given mantissa.
 */
template <typename T>
void bench_all_exponents(nanobench::Bench& bench,
  std::string_view const filename,
  typename teju::traits_t<T>::u1_t const mantissa) {

  using traits_t = teju::traits_t<T>;

  auto constexpr exponent_min = traits_t::exponent_min;
  auto constexpr exponent_max = traits_t::exponent_max;

  for (std::int32_t exponent = exponent_min; exponent <= exponent_max;
    ++exponent) {
    auto const binary = teju::binary_t<T>{exponent, mantissa};
    auto const value  = traits_t::to_value(binary);
    benchmark(bench, value);
  }
}

/**
 * @brief Benchmarks conversion of centred floating-point numbers to their
 *        decimal representations. Streams out detailed benchmarks results to a
 *        given file and a summary to std::cout.
 *
 * The set of values comprises a given number of randomly selected mantissas for
 * all possible binary exponents.
 *
 * @param  filename         The name of the output file.
 * @param  n_mantissas      The number of mantissas to be drawn.
 */
template <typename T>
void
benchmark_centred(std::string_view const filename, unsigned n_mantissas) {

  using traits_t  = teju::traits_t<T>;
  using u1_t      = typename traits_t::u1_t;

  auto bench = get_bench();

  auto constexpr mantissa_min = teju_pow2(u1_t, traits_t::mantissa_size - 1);
  auto constexpr mantissa_max = 2 * mantissa_min - 1;

  using distribution_t = std::uniform_int_distribution<u1_t>;
  auto  distribution   = distribution_t{mantissa_min, mantissa_max};
  auto  device         = std::mt19937_64{};

  auto constexpr exponent_min = traits_t::exponent_min;
  auto constexpr exponent_max = traits_t::exponent_max;

  while (n_mantissas--) {
    auto const mantissa = distribution(device);
    bench_all_exponents<T>(bench, filename, mantissa);
  }

  output(bench, filename);
}

TEST(float, centred) {
  benchmark_centred<float>("float_centred.csv", 10);
}

TEST(double, centred) {
  benchmark_centred<double>("double_centred.csv", 10);
}

/**
 * @brief Benchmarks conversion of uncentred floating-point numbers to their
 *        decimal representations. Streams out detailed benchmarks results to a
 *        given file and a summary to std::cout.
 *
 * The set of values comprises all uncentred values.
 *
 * @param  filename         The name of the output file.
 */
template <typename T>
void
benchmark_uncentred(std::string_view const filename) {

  using traits_t = teju::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  auto bench = get_bench();

  auto constexpr mantissa = teju_pow2(u1_t, traits_t::mantissa_size - 1);
  bench_all_exponents<T>(bench, filename, mantissa);
  output(bench, filename);
}

TEST(float, uncentred) {
  benchmark_uncentred<float>("float_uncentred.csv");
}

TEST(double, uncentred) {
  benchmark_uncentred<double>("double_uncentred.csv");
}

} // namespace <anonymous>

// On Linux, the following should help to reduce variance of benchmark results.
//
// 1) Disable CPU frequency scaling:
//
//   $ sudo cpupower frequency-set --governor performance
//
// 2) Pin the execution to one particular CPU and disable the sibling CPU.
//
//   For instance, if you choose to pin the execution to cpu2, then run:
//
//     $ cat /sys/devices/system/cpu/cpu2/topology/thread_siblings_list
//
//   Now, suppose the output is:
//
//     2,6
//
//   This means that we need to disable cpu6:
//
//     $ sudo /bin/bash -c "echo 0 > /sys/devices/system/cpu/cpu6/online"
//
//   Finally run the benchmark with the command line
//
//     $ benchmark [GTEST_OPTIONS]... 2
//
//   If more than one CPU is provided, then the last one will be used.

int
main(int argc, char* argv[]) {

  testing::InitGoogleTest(&argc, argv);

  #if defined(__unix__)

    if (argc > 1) {

      std::cout << "PID = " << getpid() << '\n';

      auto const cpu = std::strtoul(argv[argc - 1], nullptr, 10);

      if (cpu >= CPU_SETSIZE) {
        std::fprintf(stderr, "Invalid CPU\n");
        std::exit(-1);
      }

      cpu_set_t cpu_set;
      CPU_ZERO(&cpu_set);
      CPU_SET((int) cpu, &cpu_set);

      if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) == -1) {
        std::fprintf(stderr, "Can't run on CPU %d\n", (int) cpu);
        std::exit(-1);
      }

    }

  #endif

  return RUN_ALL_TESTS();
}
