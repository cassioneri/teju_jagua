/*
ieee32.c in C and C++
  gcc -O3 -std=c11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee32.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee32.c -Wall -Wextra

ieee64.c in C and C++
  gcc -O3 -std=c11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee64.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee64.c -Wall -Wextra

test.cpp
  g++ -O3 -std=c++11 -o test -I. -I./include -I ~/ryu/cassio/ryu tests/test.cpp -Wall -Wextra ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main
 */

#define DO_RYU   1
#define DO_AMARU 1

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <random>
#include <cstring>

namespace {

template <typename>
struct fp_traits_t;

template <typename T>
typename fp_traits_t<T>::fp_t
to_value(typename fp_traits_t<T>::rep_t const& ieee) {
  typename fp_traits_t<T>::suint_t uint;
  uint   = ieee.negative;
  uint <<= fp_traits_t<T>::exponent_size;
  uint  |= ieee.exponent;
  uint <<= fp_traits_t<T>::mantissa_size;
  uint  |= ieee.mantissa;
  typename fp_traits_t<T>::fp_t value;
  std::memcpy(&value, &uint, sizeof(uint));
  return value;
}

template <>
struct fp_traits_t<float> {

  using fp_t    = float;
  using suint_t = uint32_t;
  using rep_t   = ieee32_t;

  static auto constexpr exponent_size = uint32_t{8};
  static auto constexpr mantissa_size = uint32_t{23};

  static auto constexpr exponent_max  = AMARU_POW2(int32_t, exponent_size) - 1;
  static auto constexpr mantissa_max  = AMARU_POW2(suint_t, mantissa_size);

  static rep_t
  to_ieee(fp_t value) {
    return to_ieee32(value);
  }

  static rep_t
  ryu(rep_t const& ieee) {
    auto const ryu = f2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(rep_t const& ieee) {
    return amaru_float(to_value<float>(ieee));
  }
};

template <>
struct fp_traits_t<double> {

  using fp_t    = double;
  using suint_t = uint64_t;
  using rep_t   = ieee64_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};
  static auto constexpr exponent_max  = AMARU_POW2(int32_t, exponent_size) - 1;
  static auto constexpr mantissa_max  = AMARU_POW2(suint_t, mantissa_size);

  static rep_t
  to_ieee(fp_t value) {
    return to_ieee64(value);
  }

  static rep_t
  ryu(rep_t const& ieee) {
    auto const ryu = d2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(rep_t const& ieee) {
    return amaru_double(to_value<double>(ieee));
  }
};

template <typename T>
void compare_to_ryu(typename fp_traits_t<T>::rep_t const& ieee) {

  using traits_t = fp_traits_t<T>;

  #if DO_RYU
    auto const ryu_dec = traits_t::ryu(ieee);
    (void) ryu_dec;
  #endif

  #if DO_AMARU
    auto const amaru_dec = traits_t::amaru(ieee);
    (void) amaru_dec;
  #endif

  #if DO_RYU && DO_AMARU
    EXPECT_EQ(ryu_dec.exponent, amaru_dec.exponent) << "Note: "
      "ieee.exponent = " << ieee.exponent << ", "
      "ieee.mantissa = " << ieee.mantissa;

    EXPECT_EQ(ryu_dec.mantissa, amaru_dec.mantissa) << "Note: "
        "ieee.exponent = " << ieee.exponent << ", "
        "ieee.mantissa = " << ieee.mantissa;
  #endif
}

TEST(float_tests, exhaustive_comparison_to_ryu) {

  using traits_t = fp_traits_t<float>;

  for (int32_t exponent = 0; !HasFailure() && exponent < traits_t::exponent_max;
    ++exponent) {

    #if DO_RYU && DO_AMARU
      std::cerr << "Exponent: " << exponent << std::endl;
    #endif

    for (traits_t::suint_t mantissa = exponent == 0;
      mantissa < traits_t::mantissa_max; ++mantissa) {

      auto const ieee = traits_t::rep_t{false, exponent, mantissa};
      compare_to_ryu<traits_t::fp_t>(ieee);
    }
  }
}

TEST(double_tests, random_comparison_to_ryu) {

  using traits_t = fp_traits_t<double>;

  // Ad hoc test
  if (false) {
    compare_to_ryu<traits_t::fp_t>({false, 582, 3372137371404177});
    return;
  }

  traits_t::suint_t uint_max;
  auto const fp_max = std::numeric_limits<traits_t::fp_t>::max();
  memcpy(&uint_max, &fp_max, sizeof(fp_max));

  std::random_device rd;
  auto dist = std::uniform_int_distribution<traits_t::suint_t>{1, uint_max};

  auto number_of_tests = uint32_t{100000000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (!HasFailure() && number_of_tests --> 0) {
    auto const i = dist(rd);
    traits_t::fp_t x;
    memcpy(&x, &i, sizeof(i));
    auto const ieee = traits_t::to_ieee(x);
    compare_to_ryu<traits_t::fp_t>(ieee);
    if (HasFailure())
      FAIL() << x;
  }
}

} // namespace <anonymous>
