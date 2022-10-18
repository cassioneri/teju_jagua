#include "amaru/common.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "other/other.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <iostream>
#include <random>

#include <sys/types.h>
#include <unistd.h>

struct stats_t {

  void
  update(std::uint64_t const x) {
    n_              += 1;
    sum_            += x;
    sum_of_squares_ += x * x;
  }

  std::uint64_t
  mean() const {
    return sum_ / n_;
  }

  std::uint64_t
  stddev() const {
    auto const num = n_ * sum_of_squares_ - sum_ * sum_;
    auto const den = n_ * (n_ - 1);
    return std::sqrt(double(num) / double(den));
  }

private:
  std::uint64_t n_              = 0;
  std::uint64_t sum_            = 0;
  std::uint64_t sum_of_squares_ = 0;
};

template <typename>
struct fp_traits_t;

template <>
struct fp_traits_t<float> {

  using limb_t = amaru32_limb1_t;

  static auto constexpr exponent_size = std::uint32_t{8};
  static auto constexpr mantissa_size = std::uint32_t{23};

  static void
  amaru_compact(float const value) {
    amaru_from_float_to_decimal_compact(value);
  }

  static void
  amaru_full(float const value) {
    amaru_from_float_to_decimal_full(value);
  }

  static void
  dragonbox_compact(float const value) {
    amaru::dragonbox_compact::to_decimal(value);
  }

  static void
  dragonbox_full(float const value) {
    amaru::dragonbox_full::to_decimal(value);
  }
};

template <>
struct fp_traits_t<double> {

  using limb_t = amaru64_limb1_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};

  static void
  amaru_compact(double const value) {
    amaru_from_double_to_decimal_compact(value);
  }

  static void
  amaru_full(double const value) {
    amaru_from_double_to_decimal_full(value);
  }

  static void
  dragonbox_compact(double const value) {
    amaru::dragonbox_compact::to_decimal(value);
  }

  static void
  dragonbox_full(double const value) {
    amaru::dragonbox_full::to_decimal(value);
  }
};

template <typename T>
T
from_ieee(std::uint32_t exponent, typename fp_traits_t<T>::limb_t mantissa) {

  using        traits_t = fp_traits_t<T>;
  using        limb_t   = typename traits_t::limb_t;
  limb_t const i        = (limb_t(exponent) << traits_t::mantissa_size) |
    mantissa;

  T value;
  std::memcpy(&value, &i, sizeof(i));

  return value;
}

template <typename T>
std::uint64_t
benchmark(T value, void (*function)(T)) {

  using clock_t = std::chrono::steady_clock;
  static_assert(std::is_same<clock_t::duration, std::chrono::nanoseconds>{},
    "Unsupported clock resolution.");

  auto minimum = std::uint64_t( -1);
  auto n       = std::uint32_t(256);

  do {
    auto  const start = clock_t::now();
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    auto const end = clock_t::now();
    auto const dt  = 1000 * std::uint64_t((end - start).count()) / 8;
    if (dt < minimum) {
      minimum = dt;
      n = 256;
    }
  } while (n--);

  return minimum;
}

template <typename T>
void
benchmark() {

  std::cout.precision(std::numeric_limits<T>::digits10 + 2);
//   std::cout << "exponent, mantissa, integer, value, amaru\\\\_compact, "
//     "amaru\\\\_full, dragonbox\\\\_compact, dragonbox\\\\_full\n";

  using traits_t          = fp_traits_t<T>;
  using limb_t            = typename traits_t::limb_t;
  auto const exponent_max = AMARU_POW2(limb_t, traits_t::exponent_size) - 1;
  auto const mantissa_max = AMARU_POW2(limb_t, traits_t::mantissa_size) - 1;

  std::mt19937_64 device;
  auto dist = std::uniform_int_distribution<limb_t> {1, mantissa_max};
  auto n    = std::uint32_t{256};

  stats_t amaru_compact_stats, amaru_full_stats, dragonbox_compact_stats,
    dragonbox_full_stats;

  while (n--) {

    // Force mantissa = 0 to be in the set.
    auto const mantissa = n == 0 ? 0 : dist(device);

    // If exponent == exponent_max, then value is infinity or NaN. Hence, we
    // exclude exponent_max.
    for (std::uint32_t exponent = 0; exponent < exponent_max; ++exponent) {

      auto const value = from_ieee<T>(exponent, mantissa);

      auto const amaru_compact = benchmark(value, &traits_t::amaru_compact);
      amaru_compact_stats.update(amaru_compact);

      auto const amaru_full = benchmark(value, &traits_t::amaru_full);
      amaru_full_stats.update(amaru_full);

      auto const dragonbox_compact = benchmark(value,
        &traits_t::dragonbox_compact);
      dragonbox_compact_stats.update(dragonbox_compact);

      auto const dragonbox_full = benchmark(value,
        &traits_t::dragonbox_full);
      dragonbox_full_stats.update(dragonbox_full);

//       limb_t integer;
//       std::memcpy(&integer, &value, sizeof(value));
//
//       std::cout <<
//         exponent                  << ", " <<
//         mantissa                  << ", " <<
//         integer                   << ", " <<
//         value                     << ", " <<
//         0.001 * amaru_compact     << ", " <<
//         0.001 * amaru_full        << ", " <<
//         0.001 * dragonbox_compact << ", " <<
//         0.001 * dragonbox_full    << "\n";
    }
  }

  auto const baseline = std::min(amaru_compact_stats.mean(),
    amaru_full_stats.mean());

  auto const print = [](const char* m, uint64_t const n) {
    std::cerr << m << 0.001 * n << '\n';
  };

  print("amaru_compact     (mean)   = ", amaru_compact_stats.mean  ());
  print("amaru_compact     (stddev) = ", amaru_compact_stats.stddev());
  print("amaru_compact     (rel.)   = ", 1000 *
    amaru_compact_stats.mean() / baseline);

  print("amaru_full        (mean)   = ", amaru_full_stats.mean  ());
  print("amaru_full        (stddev) = ", amaru_full_stats.stddev());
  print("amaru_full        (rel.)   = ", 1000 *
    amaru_full_stats.mean() / baseline);

  print("dragonbox_compact (mean)   = ", dragonbox_compact_stats.mean  ());
  print("dragonbox_compact (stddev) = ", dragonbox_compact_stats.stddev());
  print("dragonbox_compact (rel.)   = ", 1000 *
    dragonbox_compact_stats.mean() / baseline);

  print("dragonbox_full    (mean)   = ", dragonbox_full_stats.mean  ());
  print("dragonbox_full    (stddev) = ", dragonbox_full_stats.stddev());
  print("dragonbox_full    (rel.)   = ", 1000 *
    dragonbox_full_stats.mean() / baseline);
}

int main() {

  // Disable CPU Frequency Scaling:
  //     $ sudo cpupower frequency-set --governor performance

  // Run on CPU 2 only:
  cpu_set_t my_set;
  CPU_ZERO(&my_set);
  CPU_SET(2, &my_set);
  sched_setaffinity(getpid(), sizeof(cpu_set_t), &my_set);

  // Disable other threads/CPU running on same core as CPU 2:
  // 1) Find the other CPU that's a thread on the same core:
  //      $ cat /sys/devices/system/cpu/cpu2/topology/thread_siblings_list
  //      2,6
  //    The above means we need to disable CPU 6.
  // 2) Disable the other CPU:
  //      sudo /bin/bash -c "echo 0 > /sys/devices/system/cpu/cpu6/online"

  benchmark<float>();
}
