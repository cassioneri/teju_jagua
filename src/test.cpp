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

/**
 * \brief Given x in [0, 1[, returns the integer part of 2^32 * x.
 */
uint32_t get_x_times_2_to_32(mp_float_t const& x) {
  auto const p32  = pow(mp_float_t{2.}, 32);
  auto const ceil = static_cast<uint32_t>(x * p32);
  return ceil;
}

/**
 * \brief Primary template function that returns the integer part of
 * log_{B1}(B2^n).
 */
template <unsigned B1, unsigned B2>
int32_t logB1_powB2(int32_t n);

/**
 * \brief Returns the integer part of log_10(2^n).
 *
 * This is a fast implementation under test. The maximal internal on which it
 * gives a correct result is to be found.
 */
template <>
int32_t logB1_powB2<10, 2>(int32_t n) {
  return log10_pow2(n) ;
}

/**
 * \brief Returns the integer part of log_5(2^n).
 *
 * This is a fast implementation under test. The maximal internal on which it
 * gives a correct result is to be found.
 */
template <>
int32_t logB1_powB2<5, 2>(int32_t n) {
  return log5_pow2(n) ;
}

/**
 * \brief Returns the integer part of log_2(10^n).
 *
 * This is a fast implementation under test. The maximal internal on which it
 * gives a correct result is to be found.
 */
template <>
int32_t logB1_powB2<2, 10>(int32_t n) {
  return log2_pow10(n) ;
}

/**
 * \brief Tests the fast implementation of the integer part of log_{B1}(B2^n)
 * given by
 *
 *    M * n >> K,
 *
 * where the multiplier M is a 32-bits approximation of log_{B1}(B2). Notice
 * that the K lower bits of M * n correspond to the fractional part of
 * log_{B1}(B2^n).
 *
 * This function tests that the implementation is correct in [min, max[ and that
 * these bounds are sharp.
 *
 * \pre 0 <= K && K <= 32.
 *
 * \tparam B1               1st base.
 * \tparam B2               2nd base.
 * \tparam K                Number of bits.
 *
 * \param  multiplier       The multiplier M.
 * \param  min              The minimum bound (inclusive).
 * \param  max              The maximum bound (non inclusive).
 */
template <unsigned B1, unsigned B2, unsigned K>
void test_log(uint64_t const multiplier, int32_t const min, int32_t const max) {
  {
    // Tests n from 0 (inclusive) to min (inclusive).

    // Loop invariant: B1^correct    <= B2^n    < B1^(correct + 1)
    //                 B1^(-correct) >= B2^(-n) > B1^(-correct - 1)

    // For n == 0:
    auto correct = int32_t{0};
    auto powB1   = mp_int_t{1}; // B1^(-correct)
    auto powB2   = mp_int_t{1}; // B2^n

    // TIP: Not stopping at n = min is useful to discover what should be the
    // value of min.
    for (int32_t n = 0; n >= min; --n) {

      // Test the real code.
      ASSERT_EQ((logB1_powB2<B1, B2>(n)), correct) << "Note n = " << n;

      auto const approximation = int32_t(multiplier * n >> K);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      // Restore loop invariant for next iteration.
      powB2 *= B2;
      while (powB1 < powB2) {
        powB1 *= B1;
        --correct;
      }
    }

    // Tests whether min is sharp.

    auto const n = min - 1;

    EXPECT_NE((logB1_powB2<B1, B2>(n)), correct) <<
      "Minimum " << min << " isn't sharp.";

    auto const approximation = int32_t(multiplier * n >> K);
    EXPECT_NE(correct, approximation) << "Minimum " << min << " isn't sharp.";
  }
  {
    // Tests for n from 0 (inclusive) to max (non inclusive).

    // Loop invariant: B1^correct <= B2^n < B1^(correct + 1)

    // n == 0:
    auto correct = int32_t{0};
    auto powB1   = mp_int_t{B1}; // B1^(correct + 1)
    auto powB2   = mp_int_t{1};  // B2^n

    // TIP: Not stopping at n = max is useful to discover what should be the
    // value of max.
    for (int32_t n = 0; n < max; ++n) {

      // Test the real code.
      ASSERT_EQ((logB1_powB2<B1, B2>(n)), correct) << "Note n = " << n;

      auto const approximation = int32_t(multiplier * n >> K);
      ASSERT_EQ(approximation, correct) << "Note n = " << n;

      // Restore loop invariant for next iteration.
      powB2 *= B2;
      while (powB1 <= powB2) {
        powB1 *= B1;
        ++correct;
      }
    }

    // Tests whether max is sharp.

    auto const n = max;
    EXPECT_NE((logB1_powB2<B1, B2>(n)), correct)
      << "Maximum " << max << " isn't sharp.";

    auto const approximation = int32_t(multiplier * n >> K);
    EXPECT_NE(correct, approximation) << "Maximum " << max << " isn't sharp.";
  }
}

TEST(log_tests, log10_pow2) {

  auto const log10_2 =
    mp_float_t{".30102999566398119521373889472449302676818988146210"};

  auto const log10_2_times_2_to_32 = get_x_times_2_to_32(log10_2);
  EXPECT_EQ(log10_2_times_2_to_32, 1292913986);
  test_log<10, 2, 32>(log10_2_times_2_to_32, -70776, 70777);
}

TEST(log_tests, log5_pow2) {

  auto const log5_2 =
    mp_float_t{".43067655807339305067010656876396563206979193207975"};

  auto const log5_2_times_2_to_32 = get_x_times_2_to_32(log5_2);
  EXPECT_EQ(log5_2_times_2_to_32, 1849741732);
  test_log<5, 2, 32>(log5_2_times_2_to_32, -227267, 227268);
}

TEST(log_tests, log2_pow10) {

  auto const log2_10 =
    mp_float_t{"3.32192809488736234787031942948939017586483139302460"};

  // Since log2_10 < 4, we divide it by 4 to get a result in [0, 1[, a
  // pre-condition of get_32_bits_approximation. Consequently, the 30 lower bits
  // of the multiplier correspond to the fractional part.

  auto const log2_10_times_2_to_30 = get_x_times_2_to_32(log2_10 / 4);
  EXPECT_EQ(log2_10_times_2_to_30, 3566893131);
  test_log<2, 10, 30>(log2_10_times_2_to_30, -55266, 55267);
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
