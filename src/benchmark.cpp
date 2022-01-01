#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>
#include <iostream>
#include <random>

#include <sys/types.h>
#include <unistd.h>

template <typename>
struct fp_traits_t;

template <typename T>
typename fp_traits_t<T>::rep_t
to_ieee(T const value) {

  using traits_t = fp_traits_t<T>;

  typename traits_t::suint_t i;
  memcpy(&i, &value, sizeof(value));

  typename fp_traits_t<T>::rep_t ieee;
  ieee.mantissa = AMARU_LSB(i, traits_t::mantissa_size);
  i >>= traits_t::mantissa_size;
  ieee.exponent = AMARU_LSB(i, traits_t::exponent_size);
  i >>= traits_t::exponent_size;
  ieee.negative = i;

  return ieee;
}

template <typename T>
T
from_ieee(uint32_t exponent, typename fp_traits_t<T>::suint_t mantissa) {

  using         traits_t  = fp_traits_t<T>;
  using         suint_t   = typename traits_t::suint_t;
  suint_t const i         = (suint_t(exponent) << traits_t::mantissa_size) |
    mantissa;

  T value;
  memcpy(&value, &i, sizeof(i));

  return value;
}

template <>
struct fp_traits_t<float> {

  using fp_t    = float;
  using suint_t = uint32_t;
  using rep_t   = ieee32_t;

  static auto constexpr exponent_size = uint32_t{8};
  static auto constexpr mantissa_size = uint32_t{23};

  static void
  ryu(fp_t const value) {
    auto ieee = to_ieee(value);
    f2d(ieee.mantissa, ieee.exponent);
  }

  static void
  amaru(fp_t const value) {
    to_amaru_dec_float(value);
  }
};

template <>
struct fp_traits_t<double> {

  using fp_t    = double;
  using suint_t = uint64_t;
  using rep_t   = ieee64_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};

  static void
  ryu(fp_t const value) {
    auto ieee = to_ieee(value);
    d2d(ieee.mantissa, ieee.exponent);
  }

  static void
  amaru(fp_t const value) {
    to_amaru_dec_double(value);
  }
};

template <typename T>
void benchmark() {

  using ns_t               = std::chrono::nanoseconds;
  std::cout.precision(std::numeric_limits<T>::digits10 + 2);
  std::cout << "exponent, mantissa, integer, value, amaru, ryu\n";

  using traits_t          = fp_traits_t<T>;
  using suint_t           = typename traits_t::suint_t;
  auto const exponent_max = AMARU_POW2(suint_t, traits_t::exponent_size) - 1;
  auto const mantissa_max = AMARU_POW2(suint_t, traits_t::mantissa_size) - 1;

  //std::random_device device;
  std::mt19937_64 device;
  auto dist = std::uniform_int_distribution<suint_t> {0, mantissa_max};

  auto           n_mantissas  = uint32_t{1000};
  auto constexpr n_iterations = uint32_t{100};

  uint64_t amaru_total = 0;
  uint64_t ryu_total   = 0;
  uint64_t count       = 0;

  while (n_mantissas--) {

    auto const mantissa = dist(device);

    #if 0
    uint32_t exponent = 149;
    (void) exponent_max;
    {
    #else
    // If exponent == exponent_max, then value is infinity or NaN. Hence, we
    // exclude exponent_max.
    for (uint32_t exponent = 0; exponent < exponent_max; ++exponent) {
    #endif
      auto const value = from_ieee<T>(exponent, mantissa);

      auto amaru = ns_t{std::numeric_limits<ns_t::rep>::max()};
      for (auto n = n_iterations; n != 0; --n) {
        auto const start = std::chrono::steady_clock::now();
        traits_t::amaru(value);
        auto const end   = std::chrono::steady_clock::now();
        auto const dt    = ns_t{end - start};
        if (amaru > dt)
          amaru = dt;
      }

      auto ryu = ns_t{std::numeric_limits<ns_t::rep>::max()};
      for (auto n = n_iterations; n != 0; --n) {
        auto const start = std::chrono::steady_clock::now();
        traits_t::ryu(value);
        auto const end   = std::chrono::steady_clock::now();
        auto const dt    = ns_t{end - start};
        if (ryu > dt)
          ryu = dt;
      }

      suint_t integer;
      std::memcpy(&integer, &value, sizeof(value));

      std::cout <<
        exponent      << ", " <<
        mantissa      << ", " <<
        integer       << ", " <<
        value         << ", " <<
        amaru.count() << ", " <<
        ryu.count()   << '\n';

      ++count;
      amaru_total += amaru.count();
      ryu_total   += ryu.count();
    }
  }

  auto avg = [=](uint64_t total) {
    return double(total) / count;
  };

  std::cerr << "amaru (avg) = " << avg(amaru_total) << '\n';
  std::cerr << "ryu (avg)   = " << avg(ryu_total   ) << '\n';
  std::cerr << "speed up    = " << avg(ryu_total) / avg(amaru_total) << '\n';
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
