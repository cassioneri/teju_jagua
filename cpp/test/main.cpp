// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "teju/common.h"
#include "teju/config.h"
#include "teju/double.h"
#include "teju/float.h"
#include "teju/float128.h"
#include "teju/literal.h"
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"
#include "cpp/common/dragonbox.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>

namespace {

using namespace teju;
using namespace teju::test;

/**
 * @brief Creates an integer number from the decimal representation (chars).
 *
 * @tparam T
 * @tparam T                The type of the number to be generated.
 * @tparam Cs...            The chars of decimal representation of the number.
 */
template <typename T, char... Cs>
struct make_number;

// Specialisation when the pack of chars is empty.
template <typename T>
struct make_number<T> {
  static T value(T n) {
    return n;
  }
};

// Generic implementation.
template <typename T, char C, char... Cs>
struct make_number<T, C, Cs...> {

  /**
   * @brief Appends to n the digits C, Cs...
   *
   * For instance, for n = 123 and the pack '4', '5', '6', returns the integer
   * 123456.
   *
   * @param n                 The value of n.
   */
  static T value(T n) {
    return make_number<T, Cs...>::value(10 * n + (C - '0'));
  }
};

/**
 * @brief Gets the next value of type T following a given one.
 *
 * @pre value >= 0.
 *
 * @tparam T                The floating point value type.
 * @param  value            The given value.
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
 * @brief Converts a given value from binary to decimal using Teju Jagua and a
 * third part-library and check whether they match.
 *
 * @tparam T                The floating point value type.
 * @param  value            The given value.
 */
template <typename T>
void compare_to_others(T const value) {

  using      traits_t  = teju::traits_t<T>;
  auto const teju      = traits_t::teju(value);
  auto const ryu       = traits_t::ryu(value);
  auto const dragonbox = traits_t::dragonbox(value);
  auto const fields    = traits_t::value_to_ieee(value);

  EXPECT_EQ(ryu      , teju) << "IEEE fields: " << fields;
  EXPECT_EQ(dragonbox, teju) << "IEEE fields: " << fields;
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_others) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<std::int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = traits_t<float>::value_to_ieee(value);
    if (ieee.exponent != exponent) {
      exponent = ieee.exponent;
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
// parameterized on the floating point number type and is instantiated for float
// and double.
TYPED_TEST_P(typed_tests_t, mantissa_min_all_exponents) {

  using traits_t = teju::traits_t<TypeParam>;
  using fp_t     = TypeParam;
  using u1_t     = typename traits_t::u1_t;

  auto const exponent_max = (std::uint32_t{1} << traits_t::exponent_size) - 1;

  for (std::uint32_t exponent = 1; !this->HasFailure() &&
    exponent < exponent_max; ++exponent) {

    auto const bits = u1_t{exponent} << (traits_t::mantissa_size - 1);
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

  auto const max   = std::pow(fp_t{2}, fp_t{traits_t<fp_t>::mantissa_size - 1});
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

  using traits_t    = teju::traits_t<float16_t>;
  using fields_t    = cpp_fields_t<float16_t>;
  using test_case_t = teju::test::test_case_t<float16_t>;

  struct test_data_t {
    float16_t value;
    fields_t  decimal;
    int       line;
  };

  test_data_t data[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    //     value, { mantissa, exponent }, line
    {     1.0f16, {        1,        0 }, __LINE__ },
    {     2.0f16, {        2,        0 }, __LINE__ },
    {     3.0f16, {        3,        0 }, __LINE__ },
    {     4.0f16, {        4,        0 }, __LINE__ },
    {     5.0f16, {        5,        0 }, __LINE__ },
    {     6.0f16, {        6,        0 }, __LINE__ },
    {     7.0f16, {        7,        0 }, __LINE__ },
    {     8.0f16, {        8,        0 }, __LINE__ },
    {     9.0f16, {        9,        0 }, __LINE__ },
    {    10.0f16, {        1,        1 }, __LINE__ },
    {    11.0f16, {       11,        0 }, __LINE__ },
    {    20.0f16, {        2,        1 }, __LINE__ },
    {   100.0f16, {        1,        2 }, __LINE__ },
    {  1000.0f16, {        1,        3 }, __LINE__ },
    { 10000.0f16, {        1,        4 }, __LINE__ },

    // 1037 * 2^4 : s == b = 1660, b is a tie, m is odd  => take closest
    { 16592.0f16, {     1659,        1 }, __LINE__ },
    // 1038 * 2^4 : s == a = 1660, a is a tie, m is even => take s.
    { 16608.0f16, {      166,        2 }, __LINE__ },

    { 65504.0f16, {      655,        2 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    //     value, { mantissa, exponent }, line
    {  0.5000f16, {        5,       -1 }, __LINE__ },
    {  0.2500f16, {       25,       -2 }, __LINE__ },
    {  0.1250f16, {      125,       -3 }, __LINE__ },
    {  0.7500f16, {       75,       -2 }, __LINE__ },

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    //     value, { mantissa, exponent }, line
    {  0.3000f16, {        3,       -1 }, __LINE__ },
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const value     = data[i].value;
    auto const test_case = test_case_t{value, data[i].decimal};
    auto const actual    = traits_t::teju(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case line = " << data[i].line;
  }
}

#if 0
TEST(float16, test_hard_coded_binary_representations) {

  using traits_t    = teju::traits_t<float128_t>;
  using fields_t    = traits_t::fields_t;
  using test_case_t = teju::test::test_case_t<float128_t>;

  static auto constexpr teju_size = std::uint32_t{128};

  struct test_data_t {
    fields_t binary;
    fields_t decimal;
  };

  test_data_t data[] = {
    // Binary                                                Decimal
    //                                mantissa  exponent                                     mantissa, exponent
    {{ 6230756230241792923652294673694720_u128,     -114 }, { 2999999999999999888977697537484346_u128,      -34 }},
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const test_case = test_case_t{data[i].binary, data[i].decimal};
    auto const actual    = traits_t::teju(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case number = " << i;
  }
}
#endif

#endif // defined(teju_has_float16)

#if defined(teju_has_float128)

template <char... Cs>
uint128_t operator ""_u128() {
  return make_number<uint128_t, Cs...>::value(0);
}

TEST(float128, test_hard_coded_values) {

  using traits_t    = teju::traits_t<float128_t>;
  using fields_t    = cpp_fields_t<float128_t>;
  using test_case_t = teju::test::test_case_t<float128_t>;

  static auto constexpr teju_size = std::uint32_t{128};

  struct test_data_t {
    float128_t value;
    fields_t   decimal;
  };

  test_data_t data[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    //             value                                mantissa  exponent
    {          1.0000000,                                      1,        0 },
    {          2.0000000,                                      2,        0 },
    {          3.0000000,                                      3,        0 },
    {          4.0000000,                                      4,        0 },
    {          5.0000000,                                      5,        0 },
    {          6.0000000,                                      6,        0 },
    {          7.0000000,                                      7,        0 },
    {          8.0000000,                                      8,        0 },
    {          9.0000000,                                      9,        0 },
    {         10.0000000,                                      1,        1 },
    {         11.0000000,                                     11,        0 },
    {         20.0000000,                                      2,        1 },
    {        100.0000000,                                      1,        2 },
    {       1000.0000000,                                      1,        3 },
    {      10000.0000000,                                      1,        4 },
    {     100000.0000000,                                      1,        5 },
    {    1000000.0000000,                                      1,        6 },
    {   10000000.0000000,                                      1,        7 },
    {  100000000.0000000,                                      1,        8 },
    { 1000000000.0000000,                                      1,        9 },

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    //             value                                mantissa  exponent
    {          0.5000000,                                      5,       -1 },
    {          0.2500000,                                     25,       -2 },
    {          0.1250000,                                    125,       -3 },
    {          0.7500000,                                     75,       -2 },

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    //             value                                 mantissa  exponent
    {          0.3000000, 2999999999999999888977697537484346_u128,      -34 },
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const value     = data[i].value;
    auto const test_case = test_case_t{value, data[i].decimal};
    auto const actual    = traits_t::teju(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case number = " << i;
  }
}

TEST(float128, test_hard_coded_binary_representations) {

  using traits_t    = teju::traits_t<float128_t>;
  using fields_t    = traits_t::fields_t;
  using test_case_t = teju::test::test_case_t<float128_t>;

  static auto constexpr teju_size = std::uint32_t{128};

  struct test_data_t {
    fields_t binary;
    fields_t decimal;
  };

  test_data_t data[] = {
    // Binary                                                Decimal
    //                                mantissa  exponent                                     mantissa, exponent
    {{ 6230756230241792923652294673694720_u128,     -114 }, { 2999999999999999888977697537484346_u128,      -34 }},
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const test_case = test_case_t{data[i].binary, data[i].decimal};
    auto const actual    = traits_t::teju(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case number = " << i;
  }
}

#endif // defined(teju_has_float128)

// Adhoc test for a given floating point number value.
TEST(ad_hoc, value) {
  auto const value = 1.0f;
  compare_to_others(value);
}

// Adhoc test for given field values.
TEST(ad_hoc, fields) {
  auto const value = traits_t<float>::ieee_to_value({ 127, 0 }); // = 1.0f
  compare_to_others(value);
}

} // namespace <anonymous>
