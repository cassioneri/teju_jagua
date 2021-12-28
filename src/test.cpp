#include "common.h"
#include "ieee.h"

#include "math.hpp"

#include <ryu.h>

#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <type_traits>

namespace {

using mp_float_t = boost::multiprecision::cpp_bin_float_50;
using mp_int_t   = boost::multiprecision::cpp_int;

int32_t get_multiplier(mp_float_t const& log) {
  auto const pow2size   = pow(mp_float_t{2.}, 32);
  auto const multiplier = static_cast<int32_t>(log * pow2size);
  return multiplier;
}

template <unsigned B>
int32_t logB_pow2(int32_t n);

template <>
int32_t logB_pow2<10>(int32_t n) {
  return log10_pow2(n) ;
}

template <>
int32_t logB_pow2<5>(int32_t n) {
  return log5_pow2(n) ;
}

template <unsigned B>
void test_log(uint64_t const multiplier, int32_t const min, int32_t const max) {
  {
    // n == 0:
    auto powBn   = mp_int_t{1}; // B^n
    auto pow2n   = mp_int_t{1}; // 2^n
    auto correct = int32_t{0};  // B^correct <= 2^n < B^(correct + 1)

    for (int32_t n = 0; n >= min; --n) {

      // Test the real code.
      ASSERT_EQ(logB_pow2<B>(n), correct) << "Note n = " << n;

      // Helps find the multiplier used by log10_pow2 and log5_pow2.
      auto const approximation = int32_t(multiplier * n >> 32);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      pow2n *= 2;
      if (powBn < pow2n) {
        powBn *= B;
        --correct;
      }
    }

    auto const n = min - 1;

    EXPECT_NE(logB_pow2<B>(n), correct) << "Minimum " << min << " isn't sharp.";

    auto const approximation = int32_t(multiplier * n >> 32);
    EXPECT_NE(correct, approximation) << "Minimum " << min << " isn't sharp.";
  }
  {
    // n == 0:
    auto powBnp1 = mp_int_t{B}; // B^(n + 1)
    auto pow2n   = mp_int_t{1}; // 2^n
    auto correct = int32_t{0};  // B^correct <= 2^n < B^(correct + 1)

    for (int32_t n = 0; n < max; ++n) {

      // Test the real code.
      ASSERT_EQ(logB_pow2<B>(n), correct) << "Note n = " << n;

      // Helps find the multiplier used by log10_pow2 and log5_pow2.
      auto const approximation = int32_t(multiplier * n >> 32);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      pow2n *= 2;
      if (powBnp1 < pow2n) {
        powBnp1 *= B;
        ++correct;
      }
    }

    auto const n = max;
    EXPECT_NE(logB_pow2<B>(n), correct) << "Maximum " << max << " isn't sharp.";

    auto const approximation = int32_t(multiplier * n >> 32);
    EXPECT_NE(correct, approximation) << "Maximum " << max << " isn't sharp.";
  }
}

auto const log10_2 =
  mp_float_t{".30102999566398119521373889472449302676818988146210"};

TEST(log10_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier(log10_2);
  EXPECT_EQ(multiplier, 1292913986);
  test_log<10>(multiplier, -70776, 70777);
}

auto const log5_2 =
  mp_float_t{".43067655807339305067010656876396563206979193207975"};

TEST(log5_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier(log5_2);
  EXPECT_EQ(multiplier, 1849741732);
  test_log<5>(multiplier, -227267, 227268);
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

  using          traits_t = fp_traits_t<T>;
  using          fp_t     = typename traits_t::fp_t;
  auto constexpr digits   = std::numeric_limits<fp_t>::digits10 + 1;

  auto const ryu_dec      = traits_t::ryu(value);
  auto const amaru_dec    = traits_t::amaru(value);

  auto const ieee         = to_ieee(value);

  EXPECT_EQ(ryu_dec.exponent, amaru_dec.exponent) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;

  EXPECT_EQ(ryu_dec.mantissa, amaru_dec.mantissa) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
      "ieee.exponent = " << ieee.exponent << ", "
      "ieee.mantissa = " << ieee.mantissa;
}

TEST(float_tests, exhaustive_comparison_to_ryu) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = to_ieee(value);
    if (ieee.exponent != exponent) {
      exponent = ieee.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

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
