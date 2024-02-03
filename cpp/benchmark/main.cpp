#include "amaru/common.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "cpp/benchmark/sampler.hpp"
#include "cpp/benchmark/stats.hpp"
#include "cpp/common/exception.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <iostream>

#include <sys/types.h>

#if defined(__unix__)
  #include <unistd.h>
#endif

namespace amaru {

template <typename T>
#if defined(__GNUC__) && !defined(__clang__)
  __attribute__((optimize("-falign-loops=32")))
#endif
std::uint64_t
benchmark(T const value, typename traits_t<T>::fields_t (*function)(T)) {

  using clock_t = std::chrono::steady_clock;

  auto minimum = clock_t::duration{std::numeric_limits<clock_t::rep>::max()};
  auto n       = std::uint32_t{256};

  do {
    auto const start = clock_t::now();
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    function(value);
    auto const end = clock_t::now();
    auto const dt  = 1000 * (end - start) / 8;
    if (dt < minimum) {
      minimum = dt;
      n = 256;
    }
  } while (n--);

  return minimum.count();
}

template <typename T, population_t population, typename... Args>
void
benchmark(const char* filename, Args... args) {

  auto out = std::ofstream{filename};

  out.precision(std::numeric_limits<T>::digits10 + 2);
  out << "exponent, mantissa, integer, value, amaru, dragonbox\\\\_compact, "
    "dragonbox\\\\_full\n";

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;
  auto  sampler  = sampler_t<T, population>{args...};

  stats_t amaru_stats, dragonbox_compact_stats, dragonbox_full_stats;

  while (!sampler.empty()) {

    auto const value = sampler.pop();

    // This case is not covered by the algorithms.
    if (value == T{0})
      continue;

    auto const amaru = benchmark(value, &traits_t::amaru);
    amaru_stats.update(amaru);

    auto const dragonbox_compact = benchmark(value,
      &traits_t::dragonbox_compact);
    dragonbox_compact_stats.update(dragonbox_compact);

    auto const dragonbox_full = benchmark(value,
      &traits_t::dragonbox_full);
    dragonbox_full_stats.update(dragonbox_full);

    u1_t integer;
    std::memcpy(&integer, &value, sizeof(value));

    auto const fields = traits_t::value_to_ieee(value).c;

    out <<
      fields.exponent           << ", " <<
      fields.mantissa           << ", " <<
      integer                   << ", " <<
      value                     << ", " <<
      0.001 * amaru             << ", " <<
      0.001 * dragonbox_compact << ", " <<
      0.001 * dragonbox_full    << "\n";
  }

  auto const baseline = amaru_stats.mean();

  auto const print = [](const char* m, std::uint64_t const n) {
    std::cout << m << 0.001 * n << '\n';
  };

  print("amaru             (mean)   = ", amaru_stats.mean  ());
  print("amaru             (stddev) = ", amaru_stats.stddev());
  print("amaru             (rel.)   = ", 1000 * amaru_stats.mean() / baseline);

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

void
report_usage(const char* const prog) noexcept {
  std::fprintf(stderr,
    "Usage: %s SET TYPE\n"
    "\n"
    "SET    Either help (to see this message and return) or one of the sample "
      "sets: integer, mixed, centred or uncentred.\n"
    "TYPE   The type of the sample: float or double.\n",
    prog);
}

} // namespace amaru

int main(int argc, char const* const argv[]) {

  using namespace amaru;

  try {

    auto is_double  = true;
    auto population = population_t::mixed;

    // First argument.

    require(argc >= 2, "Missing command (help, integer, mixed, centred or "
      "uncentred).");

    if (std::strncmp(argv[1], "help", 5) == 0) {
      report_usage(argv[0]);
      return 0;
    }

    else if (std::strncmp(argv[1], "integer", 8) == 0)
      population = population_t::integer;

    else if (std::strncmp(argv[1], "mixed", 6) == 0)
      population = population_t::mixed;

    else if (std::strncmp(argv[1], "centred", 8) == 0)
      population = population_t::centred;

    else if (std::strncmp(argv[1], "uncentred", 10) == 0)
      population = population_t::uncentred;

    else
      throw exception_t{"Unknow SET"};

    // 2nd argument.

    require(argc == 3, "Missing type (float or double).");

    if (std::strncmp(argv[2], "float", 6) == 0)
      is_double = false;

    else if (std::strncmp(argv[2], "double", 7) == 0)
      is_double = true;

    else
      throw exception_t{"Unknow TYPE"};

    #if defined(__unix__)

      // Disable CPU Frequency Scaling:
      //   $ sudo cpupower frequency-set --governor performance

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

    #endif

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

  catch (exception_t const& e) {
    report_error(argv[0], e.what());
  }

  catch (std::exception const& e) {
    report_error(argv[0], e.what());
  }

  catch (...) {
    report_error(argv[0], "unknown error");
  }
}
