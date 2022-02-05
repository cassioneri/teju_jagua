#include "../include/common.h"
#include "../include/ieee.h"

#include "dragonbox.hpp"

#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>

namespace {

using mp_float_t = boost::multiprecision::cpp_bin_float_50;
using mp_int_t   = boost::multiprecision::cpp_int;

TEST(log_tests, log10_pow2) {

  auto const log10_2 =
    mp_float_t{".30102999566398119521373889472449302676818988146210"};

  auto const p32        = pow(mp_float_t{2.}, 32);
  auto const multiplier = static_cast<uint32_t>(log10_2 * p32) + 1;

  EXPECT_EQ(multiplier, 1292913987);

  {
    int32_t constexpr min = -112815;

    // Tests n from 0 (inclusive) to min (inclusive).

    // Loop invariant: 10^correct    <= 2^n    < 10^(correct + 1)
    //                 10^(-correct) >= 2^(-n) > 10^(-correct - 1)

    // For n == 0:
    auto correct = int32_t{0};
    auto pow10   = mp_int_t{1}; // 10^(-correct)
    auto pow2    = mp_int_t{1}; // 2^n

    // TIP: Not stopping at n = min is useful to discover what should be the
    // value of min.
    for (int32_t n = 0; n >= min; --n) {

      // Test the real code.
      ASSERT_EQ(log10_pow2(n), correct) << "Note n = " << n;

      auto const approximation = int32_t(uint64_t(multiplier) * n >> 32);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      // Restore loop invariant for next iteration.
      pow2 *= 2;
      while (pow10 < pow2) {
        pow10 *= 10;
        --correct;
      }
    }

    // Tests whether min is sharp.

    auto const n = min - 1;

    EXPECT_NE(log10_pow2(n), correct) <<
      "Minimum " << min << " isn't sharp.";

    auto const approximation = int32_t(uint64_t(multiplier) * n >> 32);
    EXPECT_NE(correct, approximation) << "Minimum " << min << " isn't sharp.";
  }

  {
    int32_t constexpr max = 112816;

    // Tests for n from 0 (inclusive) to max (non inclusive).

    // Loop invariant: 10^correct <= 2^n < 10^(correct + 1)

    // n == 0:
    auto correct = int32_t{0};
    auto pow10   = mp_int_t{10}; // 10^(correct + 1)
    auto pow2    = mp_int_t{1};  // 2^n

    // TIP: Not stopping at n = max is useful to discover what should be the
    // value of max.
    for (int32_t n = 0; n < max; ++n) {

      // Test the real code.
      ASSERT_EQ(log10_pow2(n), correct) << "Note n = " << n;

      auto const approximation = int32_t(uint64_t(multiplier) * n >> 32);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      // Restore loop invariant for next iteration.
      pow2 *= 2;
      while (pow10 <= pow2) {
        pow10 *= 10;
        ++correct;
      }
    }

    // Tests whether max is sharp.

    auto const n = max;
    EXPECT_NE(log10_pow2(n), correct)
      << "Maximum " << max << " isn't sharp.";

    auto const approximation = int32_t(uint64_t(multiplier) * n >> 32);
    EXPECT_NE(correct, approximation) << "Maximum " << max << " isn't sharp.";
  }
}

TEST(log_tests, log10_pow2_remainder) {

  for (int32_t e = -112815; e < 112816; ++e) {

    auto const f  = log10_pow2(e);
    auto const r  = log10_pow2_remainder(e);

    // e0 is the smallest value of e such that log10_pow2(e) = f.
    auto const e0 = e - static_cast<int32_t>(r);
    auto const f0 = log10_pow2(e0);
    auto const f1 = log10_pow2(e0 - 1);

    ASSERT_EQ(f0, f) << "Note: e = " << e << ", e0 = " << e0;
    ASSERT_LT(f1, f) << "Note: e = " << e << ", e0 = " << e0;
  }
}

template <typename>
struct fp_traits_t;

template <typename T>
typename fp_traits_t<T>::rep_t
to_ieee(T const value) {

  using traits_t = fp_traits_t<T>;

  typename traits_t::suint_t i;
  std::memcpy(&i, &value, sizeof(value));

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
get_next(T value) {
  typename fp_traits_t<T>::suint_t i;
  std::memcpy(&i, &value, sizeof(value));
  ++i;
  std::memcpy(&value, &i, sizeof(value));
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
  amaru(fp_t const value) {
    return amaru_float(value);
  }

  static rep_t
  dragonbox(fp_t const value) {
    return dragonbox_float(value);
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
  amaru(fp_t const value) {
    return amaru_double(value);
  }

  static rep_t
  dragonbox(fp_t const value) {
    return dragonbox_double(value);
  }
};

template <typename T>
void compare_to_other(T const value) {

  using          traits_t = fp_traits_t<T>;
  using          fp_t     = typename traits_t::fp_t;
  auto constexpr digits   = std::numeric_limits<fp_t>::digits10 + 2;
  auto const other        = traits_t::dragonbox(value);
  auto const amaru        = traits_t::amaru(value);
  auto const ieee         = to_ieee(value);

  EXPECT_EQ(other.exponent, amaru.exponent) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;

  EXPECT_EQ(other.mantissa, amaru.mantissa) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;
}

TEST(float_tests, exhaustive_comparison_to_other) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = to_ieee(value);
    if (ieee.exponent != exponent) {
      exponent = ieee.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

    compare_to_other(value);

    value = get_next(value);
  }
}

TEST(double_tests, random_comparison_to_other) {

  using traits_t = fp_traits_t<double>;

  traits_t::suint_t uint_max;
  auto const fp_max = std::numeric_limits<traits_t::fp_t>::max();
  std::memcpy(&uint_max, &fp_max, sizeof(fp_max));

  std::random_device rd;
  auto dist = std::uniform_int_distribution<traits_t::suint_t>{1, uint_max};

  auto number_of_tests = uint32_t{100000000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (!HasFailure() && number_of_tests --> 0) {
    auto const i = dist(rd);
    traits_t::fp_t value;
    std::memcpy(&value, &i, sizeof(i));
    compare_to_other(value);
  }
}

TEST(ad_hoc_test, a_particular_case) {
  auto const value = 1.f;
  compare_to_other(value);
}

} // namespace <anonymous>
