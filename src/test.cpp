/*
Compile ieee32.c with gcc and g++
  gcc -O3 -std=c11 -I. -I./include -c generated/ieee32.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -c generated/ieee32.c -Wall -Wextra

Compile ieee64.c with gcc and g++
  gcc -O3 -std=c11 -I. -I./include -c generated/ieee64.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -c generated/ieee64.c -Wall -Wextra

Compile test.cpp
  g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -o test src/test.cpp -Wall -Wextra ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

All with gcc and g++
  gcc -O3 -std=c11 -I. -I./include -c generated/ieee32.c -Wall -Wextra && gcc -O3 -std=c11 -I. -I./include -c generated/ieee64.c -Wall -Wextra && g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -o test src/test.cpp -Wall -Wextra ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

  g++ -O3 -std=c++11 -I. -I./include -c generated/ieee32.c -Wall -Wextra && g++ -O3 -std=c++11 -I. -I./include -c generated/ieee64.c -Wall -Wextra && g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -o test src/test.cpp -Wall -Wextra ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main
 */

#define DO_RYU   1
#define DO_AMARU 1

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <cmath>
#include <cstdint>
#include <random>
#include <cstring>

namespace {

using mp_float_t = boost::multiprecision::cpp_bin_float_50;
using mp_int_t   = boost::multiprecision::cpp_int;

template <typename T>
struct log_traits_t;

template <>
struct log_traits_t<int32_t> {
  using                 sint = int32_t;
  using                 dint = int64_t;
  static auto constexpr size = 32;
};

template <>
struct log_traits_t<int64_t> {
  using                 sint = int64_t;
  using                 dint = __int128_t;
  static auto constexpr size = 64;
};

template <typename T>
typename log_traits_t<T>::sint get_multiplier(mp_float_t const& log) {
  using      traits_t   = log_traits_t<T>;
  auto const pow2size   = pow(mp_float_t{2.}, traits_t::size);
  auto const multiplier = static_cast<typename traits_t::sint>(log * pow2size);
  return multiplier;
}

template <typename T>
void test_log(typename log_traits_t<T>::sint multiplier,
  typename log_traits_t<T>::sint min, typename log_traits_t<T>::sint max) {

  using traits_t = log_traits_t<T>;

  auto const lower = static_cast<typename traits_t::dint>(multiplier);
  auto const upper = lower + 1;

  for (typename traits_t::sint n = 0; n >= min; --n) {
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    ASSERT_EQ(lower_bound, upper_bound) << "Note n = " << n;
  }

  for (typename traits_t::sint n = 0; n < max; ++n) {
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    ASSERT_EQ(lower_bound, upper_bound) << "Note n = " << n;
  }

  {
    auto const n = min - 1;
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    EXPECT_GT(lower_bound, upper_bound) << "Minimum " << min << " isn't sharp.";
  }

  {
    auto const n = max;
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    EXPECT_LT(lower_bound, upper_bound) << "Maximum " << max << " isn't sharp.";
  }
}

auto const log10_2 =
  mp_float_t{".30102999566398119521373889472449302676818988146210"};

TEST(log10_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier<int32_t>(log10_2);
  EXPECT_EQ(multiplier, int32_t{1292913986});
  test_log<int32_t>(multiplier, int32_t{-70776}, int32_t{70777});
}

TEST(log10_pow2_tests, for_int64_t) {
  auto const multiplier = get_multiplier<int64_t>(log10_2);
  EXPECT_EQ(multiplier, int64_t{5553023288523357132});
  test_log<int64_t>(multiplier, int64_t{-1923400329}, int64_t{1923400330});
}

auto const log5_2 =
  mp_float_t{".43067655807339305067010656876396563206979193207975"};

TEST(log5_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier<int32_t>(log5_2);
  EXPECT_EQ(multiplier, int32_t{1849741732});
  test_log<int32_t>(multiplier, int32_t{-78854}, int32_t{78855});
}

TEST(log5_pow2_tests, for_int64_t) {
  auto const multiplier = get_multiplier<int64_t>(log5_2);
  EXPECT_EQ(multiplier, int64_t{7944580245325990804});
  test_log<int64_t>(multiplier, int64_t{-1344399136}, int64_t{1344399137});
}

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

  static rep_t
  ryu(fp_t const value) {
    auto ieee = to_ieee(value);
    auto const ryu = f2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(fp_t const value) {
    return to_amaru_dec_float(value);
  }
};

template <>
struct fp_traits_t<double> {

  using fp_t    = double;
  using suint_t = uint64_t;
  using rep_t   = ieee64_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};

  static rep_t
  ryu(fp_t const value) {
    auto ieee = to_ieee(value);
    auto const ryu = d2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(fp_t const value) {
    return to_amaru_dec_double(value);
  }
};

template <typename T>
void compare_to_ryu(T const value) {

  using traits_t = fp_traits_t<T>;

  #if DO_RYU
    auto ryu_dec = traits_t::ryu(value);
    asm("" : "+r"(ryu_dec.exponent), "+r"(ryu_dec.mantissa));
  #endif

  #if DO_AMARU
    auto amaru_dec = traits_t::amaru(value);
    asm("" : "+r"(amaru_dec.exponent), "+r"(amaru_dec.mantissa));
  #endif

  #if DO_RYU && DO_AMARU

    auto const     ieee   = to_ieee(value);
    using          fp_t   = typename traits_t::fp_t;
    auto constexpr digits = std::numeric_limits<fp_t>::digits10 + 1;

    EXPECT_EQ(ryu_dec.exponent, amaru_dec.exponent) << "Note: "
      "value = " << std::setprecision(digits) << value << ", "
      "ieee.exponent = " << ieee.exponent << ", "
      "ieee.mantissa = " << ieee.mantissa;

    EXPECT_EQ(ryu_dec.mantissa, amaru_dec.mantissa) << "Note: "
      "value = " << std::setprecision(digits) << value << ", "
        "ieee.exponent = " << ieee.exponent << ", "
        "ieee.mantissa = " << ieee.mantissa;
  #endif
}

TEST(float_tests, exhaustive_comparison_to_ryu) {

  auto value    = std::numeric_limits<float>::denorm_min();
  #if DO_RYU && DO_AMARU
    auto exponent = std::numeric_limits<int32_t>::min();
  #endif

  while (std::isfinite(value) && !HasFailure()) {

    #if DO_RYU && DO_AMARU
      auto const ieee = to_ieee(value);
      if (ieee.exponent != exponent) {
        exponent = ieee.exponent;
        std::cerr << "Exponent: " << exponent << std::endl;
      }
    #endif

    compare_to_ryu(value);

    value = get_next(value);
  }
}

TEST(double_tests, random_comparison_to_ryu) {

  using traits_t = fp_traits_t<double>;

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
    traits_t::fp_t value;
    memcpy(&value, &i, sizeof(i));
    compare_to_ryu(value);
  }
}

TEST(ad_hoc_test, a_particular_case) {
  auto const value = 1.f;
  compare_to_ryu(value);
}

} // namespace <anonymous>
