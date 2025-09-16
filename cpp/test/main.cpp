// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "common/traits.hpp"
#include "teju/src/common.h"
#include "teju/src/config.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>

namespace {

using namespace teju;

/**
 * @brief Creates an integer number from chars.
 *
 * @tparam T                The type of the number to be generated.
 * @tparam Cs               The chars.
 */
template <typename T, char... Cs>
struct make_number;

// Specialisation when the pack of chars is empty.
template <typename T>
struct make_number<T> {

  /**
   * @brief Returns n.
   *
   * @param  n              The value of n.
   *
   * @returns n.
   */
  static
  T
  value(T n) {
    return n;
  }
};

// Generic implementation.
template <typename T, char C, char... Cs>
struct make_number<T, C, Cs...> {

  /**
   * @brief Appends chars to n.
   *
   * For instance, for n = 123 and the pack '4', '5', '6', it returns 123456.
   *
   * @param  n              The value of n.
   *
   * @returns The number n appended with the given chars.
   */
  static
  T
  value(T n) {
    return make_number<T, Cs...>::value(10 * n + (C - '0'));
  }
};

/**
 * @brief Gets the next value of type T following a given one.
 *
 * @tparam T                The floating-point value type.
 * @param  value            The given value.
 *
 * @pre std::isfinite(value) && value > 0.
 *
 * @returns The next value.
 */
template <typename T>
T
get_next(T value) {
  typename traits_t<T>::u1_t i;
  std::memcpy(&i, &value, sizeof(value));
  ++i;
  std::memcpy(&value, &i, sizeof(value));
  return value;
}

/**
 * @brief Converts a given value from binary to decimal using Tejú Jaguá and a
 *        third part-library and check whether they match.
 *
 * @tparam T                The floating-point value type.
 * @param  value            The given value.
 */
template <typename T>
void compare_to_others(T const value) {

  using      traits_t  = teju::traits_t<T>;

  auto const teju      = traits_t::teju(value);
  auto const ryu       = traits_t::ryu(value);
  auto const dragonbox = traits_t::dragonbox(value);

  auto const binary    = traits_t::to_binary(value);

  EXPECT_EQ(ryu      , teju) << "Value = " << binary << " ~= " << value << '.';
  EXPECT_EQ(dragonbox, teju) << "Value = " << binary << " ~= " << value << '.';
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_others) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<std::int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const binary = traits_t<float>::to_binary(value);
    if (binary.exponent != exponent) {
      exponent = binary.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

    compare_to_others(value);
    value = get_next(value);
  }
}

template <typename T>
class typed_tests_t : public testing::Test {
};

TYPED_TEST_SUITE_P(typed_tests_t);

// Test results for the minimum mantissa and all exponents. This test is
// parameterized on the floating-point number type and is instantiated for float
// and double.
TYPED_TEST_P(typed_tests_t, mantissa_min_all_exponents) {

  using fp_t     = TypeParam;
  using traits_t = teju::traits_t<fp_t>;
  using u1_t     = typename traits_t::u1_t;

  for (std::uint32_t exponent = traits_t::exponent_min; !this->HasFailure() &&
    exponent <= traits_t::exponent_max; ++exponent) {

    auto const bits = teju_pow2(u1_t, traits_t::mantissa_width - 1);
    fp_t value;
    std::memcpy(&value, &bits, sizeof(bits));
    compare_to_others(value);
  }
}

TYPED_TEST_P(typed_tests_t, integers) {

  using fp_t = TypeParam;

  auto test = [this](fp_t const min, fp_t const max) {
    for (fp_t value = min; value < max && !this->HasFailure(); ++value) {
      compare_to_others(value);
      ASSERT_NE(value + 1.0, value);
    }
  };

  auto const max   = std::pow(fp_t{2}, fp_t{traits_t<fp_t>::mantissa_width - 1});
  auto const limit = fp_t{10'000'000};

  if (max <= limit) {
    // Tests all small integers.
    test(fp_t{1}, max);
  }
  else {
    // Tests first small integers.
    test(fp_t{1}, limit);
    // Tests last  small integers.
    test(std::max(limit, max - limit), max);
  }
}

REGISTER_TYPED_TEST_SUITE_P(typed_tests_t,
  mantissa_min_all_exponents,
  integers
);

using fp_types_t = ::testing::Types<float, double>;
INSTANTIATE_TYPED_TEST_SUITE_P(typed_tests_t, typed_tests_t, fp_types_t);

// Test results for a large number of random double values.
TEST(double, random_comparison_to_other) {

  using traits_t = traits_t<double>;

  traits_t::u1_t uint_max;
  auto const double_max = std::numeric_limits<double>::max();
  std::memcpy(&uint_max, &double_max, sizeof(double_max));

  std::random_device rd;
  auto dist = std::uniform_int_distribution<traits_t::u1_t>{1, uint_max};

  auto number_of_tests = std::uint32_t{100000000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (!HasFailure() && number_of_tests --> 0) {
    auto const i = dist(rd);
    double value;
    std::memcpy(&value, &i, sizeof(i));
    compare_to_others(value);
  }
}

#if defined(teju_has_float16)

TEST(float16, test_hard_coded_values) {

  using traits_t  = teju::traits_t<float16_t>;
  using decimal_t = teju::decimal_t<float16_t>;

  struct test_data_t {
    float16_t value;
    decimal_t decimal;
    int       line;
  };

  test_data_t data[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    //     value, { exponent, mantissa },     line
    {     1.0f16, {        0,        1 }, __LINE__ },
    {     2.0f16, {        0,        2 }, __LINE__ },
    {     3.0f16, {        0,        3 }, __LINE__ },
    {     4.0f16, {        0,        4 }, __LINE__ },
    {     5.0f16, {        0,        5 }, __LINE__ },
    {     6.0f16, {        0,        6 }, __LINE__ },
    {     7.0f16, {        0,        7 }, __LINE__ },
    {     8.0f16, {        0,        8 }, __LINE__ },
    {     9.0f16, {        0,        9 }, __LINE__ },
    {    10.0f16, {        1,        1 }, __LINE__ },
    {    11.0f16, {        0,       11 }, __LINE__ },
    {    20.0f16, {        1,        2 }, __LINE__ },
    {   100.0f16, {        2,        1 }, __LINE__ },
    {  1000.0f16, {        3,        1 }, __LINE__ },
    { 10000.0f16, {        4,        1 }, __LINE__ },

    // 1037 * 2^4 : s == b = 1660, b is a tie, m is odd  => take closest
    { 16592.0f16, {        1,     1659 }, __LINE__ },
    // 1038 * 2^4 : s == a = 1660, a is a tie, m is even => take s.
    { 16608.0f16, {        2,      166 }, __LINE__ },

    { 65504.0f16, {        2,      655 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    //     value, { exponent, mantissa },     line
    {  0.5000f16, {       -1,        5 }, __LINE__ },
    {  0.2500f16, {       -2,       25 }, __LINE__ },
    {  0.1250f16, {       -3,      125 }, __LINE__ },
    {  0.7500f16, {       -2,       75 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    //     value, { exponent, mantissa },     line
    {  0.3000f16, {       -1,        3 }, __LINE__ },
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const value    = data[i].value;
    auto const expected = data[i].decimal;
    auto const actual   = traits_t::teju(value);
    ASSERT_EQ(expected, actual) <<
      "    Note: test case line = " << data[i].line;
  }
}

#endif // defined(teju_has_float16)

#if defined(teju_has_float128)

template <char... Cs>
uint128_t operator ""_u128() {
  return make_number<uint128_t, Cs...>::value(0);
}

TEST(float128, test_hard_coded_values) {

  using traits_t  = teju::traits_t<float128_t>;
  using decimal_t = teju::decimal_t<float128_t>;

  static auto constexpr teju_size = std::uint32_t{128};

  struct test_data_t {
    float128_t value;
    decimal_t  decimal;
    int        line;
  };

  test_data_t data[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    //             value, { exponent,                               mantissa },     line
    {          1.0000000, {        0,                                      1 }, __LINE__ },
    {          2.0000000, {        0,                                      2 }, __LINE__ },
    {          3.0000000, {        0,                                      3 }, __LINE__ },
    {          4.0000000, {        0,                                      4 }, __LINE__ },
    {          5.0000000, {        0,                                      5 }, __LINE__ },
    {          6.0000000, {        0,                                      6 }, __LINE__ },
    {          7.0000000, {        0,                                      7 }, __LINE__ },
    {          8.0000000, {        0,                                      8 }, __LINE__ },
    {          9.0000000, {        0,                                      9 }, __LINE__ },
    {         10.0000000, {        1,                                      1 }, __LINE__ },
    {         11.0000000, {        0,                                     11 }, __LINE__ },
    {         20.0000000, {        1,                                      2 }, __LINE__ },
    {        100.0000000, {        2,                                      1 }, __LINE__ },
    {       1000.0000000, {        3,                                      1 }, __LINE__ },
    {      10000.0000000, {        4,                                      1 }, __LINE__ },
    {     100000.0000000, {        5,                                      1 }, __LINE__ },
    {    1000000.0000000, {        6,                                      1 }, __LINE__ },
    {   10000000.0000000, {        7,                                      1 }, __LINE__ },
    {  100000000.0000000, {        8,                                      1 }, __LINE__ },
    { 1000000000.0000000, {        9,                                      1 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    //             value  { exponent,                               mantissa },     line
    {          0.5000000, {       -1,                                      5 }, __LINE__ },
    {          0.2500000, {       -2,                                     25 }, __LINE__ },
    {          0.1250000, {       -3,                                    125 }, __LINE__ },
    {          0.7500000, {       -2,                                     75 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    //             value  { exponent,                                mantissa },    line
    {          0.3000000, {      -34, 2999999999999999888977697537484346_u128 }, __LINE__ },
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const value    = data[i].value;
    auto const expected = data[i].decimal;
    auto const actual   = traits_t::teju(value);
    ASSERT_EQ(expected, actual) <<
      "    Note: test case line = " << data[i].line;
  }
}

#endif // defined(teju_has_float128)

// Adhoc test for a given floating-point number value.
TEST(ad_hoc, value) {
  auto constexpr value = 1.0;
  compare_to_others(value);
}

} // namespace <anonymous>
