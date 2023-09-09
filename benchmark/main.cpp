#include "amaru/common.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "benchmark/sampler.hpp"
#include "benchmark/stats.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>

namespace amaru {

template <typename T>
#if defined(__GNUC__) && !defined(__clang__)
__attribute__((optimize("-falign-loops=32")))
#endif
std::uint64_t
benchmark(T const value, void (*function)(T)) {

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

template <typename T, population_t population, typename... Args>
void
benchmark(const char* filename, Args... args) {

  auto out = std::ofstream{filename};

  out.precision(std::numeric_limits<T>::digits10 + 2);
  out << "exponent, mantissa, integer, value, amaru\\\\_compact, "
    "amaru\\\\_full, dragonbox\\\\_compact, dragonbox\\\\_full\n";

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;
  auto  sampler  = sampler_t<T, population>{args...};

  stats_t amaru_compact_stats, amaru_full_stats, dragonbox_compact_stats,
    dragonbox_full_stats;

  while (!sampler.empty()) {

    auto const value = sampler.pop();

    // This case is not covered by the algorithms.
    if (value == T{0})
      continue;

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

    u1_t integer;
    std::memcpy(&integer, &value, sizeof(value));

    auto const fields = traits_t::to_ieee(value);

    out <<
      fields.exponent           << ", " <<
      fields.mantissa           << ", " <<
      integer                   << ", " <<
      value                     << ", " <<
      0.001 * amaru_compact     << ", " <<
      0.001 * amaru_full        << ", " <<
      0.001 * dragonbox_compact << ", " <<
      0.001 * dragonbox_full    << "\n";
  }

  auto const baseline = std::min(amaru_compact_stats.mean(),
    amaru_full_stats.mean());

  auto const print = [](const char* m, std::uint64_t const n) {
    std::cout << m << 0.001 * n << '\n';
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

template <typename T, typename U>
void
benchmark(const char* filename, population_t population, T bound,
  U n_mantissas) {

  switch (population) {
    case population_t::integer:
      return benchmark<T, population_t::integer>(filename, bound);
    case population_t::centred:
      return benchmark<T, population_t::centred>(filename, n_mantissas);
    case population_t::uncentred:
      return benchmark<T, population_t::uncentred>(filename);
    case population_t::mixed:
      return benchmark<T, population_t::mixed>(filename, n_mantissas);
  }
}

} // namespace amaru

int main(int argc, char const* const argv[]) {

  using namespace amaru;

  auto is_double  = true;
  auto population = population_t::mixed;

  while (--argc) {

    if (std::strncmp(argv[argc], "-h", 2) == 0) {
      std::cout << "usage...\n";
      std::exit(0);
    }

    else if (std::strncmp(argv[argc], "-d", 2) == 0)
      is_double = true;

    else if (std::strncmp(argv[argc], "-f", 2) == 0)
      is_double = false;

    else if (std::strncmp(argv[argc], "-integer", 8) == 0)
      population = population_t::integer;

    else if (std::strncmp(argv[argc], "-mixed", 6) == 0)
      population = population_t::mixed;

    else if (std::strncmp(argv[argc], "-centred", 8) == 0)
      population = population_t::centred;

    else if (std::strncmp(argv[argc], "-uncentred", 10) == 0)
      population = population_t::uncentred;
  }

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

  if (is_double) {
    auto constexpr n_bound = 100000.0;
    auto constexpr n_mantissas = std::uint64_t(256);
    benchmark<double>("double.csv", population, n_bound, n_mantissas);
  }
  else {
    auto constexpr n_bound = 100000.f;
    auto constexpr n_mantissas = std::uint32_t(256);
    benchmark<float>("float.csv", population, n_bound, n_mantissas);
  }
}
