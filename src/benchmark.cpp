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

#define DO_RYU   1
#define DO_AMARU 1

template <typename>
struct fp_traits_t;

template <typename T>
typename fp_traits_t<T>::rep_t
to_ieee(T const value) {

  using traits_t = fp_traits_t<T>;

  typename traits_t::suint_t i;
  memcpy(&i, &value, sizeof(value));

  typename fp_traits_t<T>::rep_t ieee;
  ieee.mantissa = AMARU_LOWER_BITS(i, traits_t::mantissa_size);
  i >>= traits_t::mantissa_size;
  ieee.exponent = AMARU_LOWER_BITS(i, traits_t::exponent_size);
  i >>= traits_t::exponent_size;
  ieee.negative = i;

  return ieee;
}

template <typename T>
T
get_next(T value) {
  typename fp_traits_t<T>::suint_t i;
  memcpy(&i, &value, sizeof(value));
  ++i;
  memcpy(&value, &i, sizeof(value));
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
    auto ryu = f2d(ieee.mantissa, ieee.exponent);
    asm("" : "+r"(ryu.exponent), "+r"(ryu.mantissa));
  }

  static void
  amaru(fp_t const value) {
    auto amaru = to_amaru_dec_float(value);
    asm("" : "+r"(amaru.exponent), "+r"(amaru.mantissa));
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
    auto ryu = d2d(ieee.mantissa, ieee.exponent);
    asm("" : "+r"(ryu.exponent), "+r"(ryu.mantissa));
  }

  static void
  amaru(fp_t const value) {
    auto amaru = to_amaru_dec_double(value);
    asm("" : "+r"(amaru.exponent), "+r"(amaru.mantissa));
  }
};

template <typename T>
void run_ryu_and_or_amaru(T const value) {

  using traits_t = fp_traits_t<T>;

  #if DO_RYU
    traits_t::ryu(value);
  #endif

  #if DO_AMARU
    traits_t::amaru(value);
  #endif
}

void run_ryu_and_or_amaru_for_all_floats() {

  auto value = std::numeric_limits<float>::denorm_min();

  while (std::isfinite(value)) {
    run_ryu_and_or_amaru(value);
    value = get_next(value);
  }
}

template <typename T>
void benchmark() {

  using traits_t = fp_traits_t<T>;
  using ns_t     = std::chrono::nanoseconds;

  std::cout.precision(std::numeric_limits<T>::digits10);
  std::cout << "integer, value, amaru, ryu\n";

  typename traits_t::suint_t uint_max;
  auto const fp_max = std::numeric_limits<T>::max();
  memcpy(&uint_max, &fp_max, sizeof(fp_max));

  std::random_device rd;
  auto dist = std::uniform_int_distribution<typename traits_t::suint_t>
    {1, uint_max};

  auto n_samples = uint32_t{10000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (n_samples --> 0) {

    auto const i = dist(rd);
    typename traits_t::fp_t value;
    memcpy(&value, &i, sizeof(i));

    auto constexpr n_iterations = uint32_t{10000};

    auto amaru = ns_t{std::numeric_limits<ns_t::rep>::max()};
    for (auto n = n_iterations; n != 0; --n) {
      auto const start = std::chrono::steady_clock::now();
      traits_t::amaru(value);
      auto const end   = std::chrono::steady_clock::now();
      auto const dt    = ns_t{end - start};
      if (dt < amaru)
        amaru = dt;
    }

    auto ryu = ns_t{std::numeric_limits<ns_t::rep>::max()};
    for (auto n = n_iterations; n != 0; --n) {
      auto const start = std::chrono::steady_clock::now();
      traits_t::ryu(value);
      auto const end   = std::chrono::steady_clock::now();
      auto const dt    = ns_t{end - start};
      if (dt < ryu)
        ryu = dt;
    }

    std::cout <<
      i             << ", " <<
      value         << ", " <<
      amaru.count() << ", " <<
      ryu.count()   << '\n';
  }
}

int main() {

  // Also disable hyperthreading with something like this:
  // cat /sys/devices/system/cpu/cpu*/topology/core_id
  // sudo /bin/bash -c "echo 0 > /sys/devices/system/cpu/cpu6/online"
  cpu_set_t my_set;
  CPU_ZERO(&my_set);
  CPU_SET(2, &my_set);
  sched_setaffinity(getpid(), sizeof(cpu_set_t), &my_set);

  benchmark<float>();
}
