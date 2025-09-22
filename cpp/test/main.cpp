// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include <charconv>

#include "common/exception.hpp"
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
 * @brief Gets the floating-point value that follows a given one.
 *
 * @tparam TFloat           The floating-point value type.
 * @param  value            The given value.
 *
 * @pre std::isfinite(value) && value >= 0.
 *
 * @returns The next value.
 */
template <typename TFloat>
TFloat
get_next(TFloat value) {
  typename traits_t<TFloat>::u1_t i;
  std::memcpy(&i, &value, sizeof(value));
  ++i;
  std::memcpy(&value, &i, sizeof(value));
  return value;
}

/**
 * @brief Gets the floating-point value that precedes a given one.
 *
 * @tparam TFloat           The floating-point value type.
 * @param  value            The given value.
 *
 * @pre std::isfinite(value) && value >= 0.
 *
 * @returns The previous value.
 */
template <typename TFloat>
TFloat
get_previous(TFloat value) {
  typename traits_t<TFloat>::u1_t i;
  std::memcpy(&i, &value, sizeof(value));
  --i;
  std::memcpy(&value, &i, sizeof(value));
  return value;
}

auto const to_chars_failure = teju::exception_t{"to_chars failed."};

/**
 * @brief Converts a floating-point number into chars in scientific format.
 *        (Writes a null terminator.)
 *
 * Simply delegates to std::to_chars and throws in case of failure.
 *
 * @tparam N                The buffer size.
 * @tparam TFloat           The floating-point number type.
 *
 * @param  chars            The chars buffer.
 * @param  value            The value to be converted.
 *
 * @returns Pointer to one-past-the-end of characters written.
 */
template <size_t N, typename TFloat>
char const* value_to_chars(char (&chars)[N], TFloat const value) {

  auto const result = std::to_chars(chars + 0, chars + N - 1, value,
    std::chars_format::scientific);

  if (result.ec == std::errc{}) {
    *result.ptr = '\0';
    return result.ptr + 1;
  }

  throw to_chars_failure;
}

/**
 * @brief Converts a given value from binary to decimal using Tejú Jaguá and a
 *        third part-library and check whether they match.
 *
 * @tparam TFloat           The floating-point value type.
 * @param  value            The given value.
 */
template <typename TFloat>
void compare_to_others(TFloat const value) {

  using      traits_t  = teju::traits_t<TFloat>;

  auto const teju      = traits_t::teju(value);
  auto const ryu       = traits_t::ryu(value);
  auto const dragonbox = traits_t::dragonbox(value);

  if (teju != ryu || teju != dragonbox) {

    auto const binary = traits_t::to_binary(value);
    char chars[100];
    value_to_chars(chars, value);

    EXPECT_EQ(ryu      , teju) << "Value = " << binary << " ~= " << chars << '.';
    EXPECT_EQ(dragonbox, teju) << "Value = " << binary << " ~= " << chars << '.';
  }
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_others) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<std::int32_t>::min();

  while (0.f < value && std::isfinite(value) && !HasFailure()) {

    auto const binary = traits_t<float>::to_binary(value);
    if (binary.exponent != exponent) {
      exponent = binary.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

    compare_to_others(value);
    value = get_next(value);
  }
}

TEST(float, hard_coded_values) {

  using traits_t  = teju::traits_t<float>;
  using binary_t  = teju::binary_t<float>;
  using decimal_t = teju::decimal_t<float>;

  using limits_t  = std::numeric_limits<float>;
  auto constexpr denorm_min = limits_t::denorm_min();
  auto constexpr min        = limits_t::min();
  auto constexpr max        = limits_t::max();
  auto constexpr epsilon    = limits_t::epsilon();

  struct test_data_t {
    float     value;
    binary_t  binary;
    decimal_t decimal;
    int       line;
  };
  test_data_t data[] = {

    //--------------------------------------------------------------------------
    // Special values: denorm_min, min, max and epsilon.
    //--------------------------------------------------------------------------

    // value is centred, neither a nor b can be a tie, s <= a.
    // value is closer to left decimal.
    {     denorm_min, {  -149,        1 }, {   -45,        1 }, __LINE__ },

    // value is centred, neither a nor b can be a tie, s <= a.
    // value is closer to right decimal.
    {            min, {  -149,  8388608 }, {   -45, 11754944 }, __LINE__ },

    // value is centred, neither a nor b can be a tie, s <= a.
    // value is closer to right decimal.
    {            max, {  +104, 16777215 }, {   +31, 34028235 }, __LINE__ },

    // value is uncentred, a < b, neither a nor b can be a tie, s <= a.
    // c == a, a isn't a tie.
    {        epsilon, {   -46,  8388608 }, {   -14, 11920929 }, __LINE__ },

    //--------------------------------------------------------------------------
    // Value is small integer.
    //--------------------------------------------------------------------------

    {            1.f, {   -23,  8388608 }, {    +0,        1 }, __LINE__ },
    {            2.f, {   -22,  8388608 }, {    +0,        2 }, __LINE__ },
    {            3.f, {   -22, 12582912 }, {    +0,        3 }, __LINE__ },
    {            4.f, {   -21,  8388608 }, {    +0,        4 }, __LINE__ },
    {            5.f, {   -21, 10485760 }, {    +0,        5 }, __LINE__ },
    {      1234567.f, {    -3,  9876536 }, {    +0,  1234567 }, __LINE__ },
    {      8388607.f, {    -1, 16777214 }, {    +0,  8388607 }, __LINE__ },
    {      8388608.f, {    +0,  8388608 }, {    +0,  8388608 }, __LINE__ },

    //--------------------------------------------------------------------------
    // Value is centred. Tests whether it can return shortest.
    //--------------------------------------------------------------------------

    // a or b can be a tie, s == b, b isn't a tie.
    {   1.34218e+08f, {    +4,  8388625 }, {    +3,   134218 }, __LINE__ },

    // a or b can be a tie, s == b, b is a tie, value wins tiebreak.
    {  3.355445e+07f, {    +2,  8388612 }, {    +1,  3355445 }, __LINE__ },

    // a or b can be a tie, s == b, b is tie, value loses tiebreak.
    { 3.3554468e+07f, {    +2,  8388617 }, {    +0, 33554468 }, __LINE__ },

    // a or b can be a tie, a < s < b.
    {  1.677722e+07f, {    +1,  8388610 }, {    +1,  1677722 }, __LINE__ },

    // a or b can be a tie, s == a, a isn't a tie.
    { 1.3421802e+08f, {    +4,  8388626 }, {    +1, 13421802 }, __LINE__ },

    // a or b can be a tie, s == a, a is a tie, value wins tiebreak.
    {  3.355447e+07f, {    +2,  8388618 }, {    +1,  3355447 }, __LINE__ },

    // a or b can be a tie, s == a, a is a tie, value loses tiebreak.
    { 3.3554452e+07f, {    +2,  8388613 }, {    +0, 33554452 }, __LINE__ },

    // a or b can be a tie, s < a.
    { 1.6777218e+07f, {    +1,  8388609 }, {    +0, 16777218 }, __LINE__ },

    // neither a nor b can be a tie, s > a.
    {  1.180592e+21f, {   +47,  8388611 }, {   +15,  1180592 }, __LINE__ },
    { 1.0485759e+06f, {    -4, 16777214 }, {    -1, 10485759 }, __LINE__ },

    // neither a nor b can be a tie, s <= a.
    { 1.1805918e+21f, {   +47,  8388609 }, {   +14, 11805918 }, __LINE__ },
    { 8.3886075e+06f, {    -1, 16777215 }, {    -1, 83886075 }, __LINE__ },

    //--------------------------------------------------------------------------
    // Value is centred. Return closest.
    //--------------------------------------------------------------------------

    // left decimal is closer.
    {  1.3421774e+08, {    +4,  8388609 }, {    +1, 13421774 }, __LINE__ },

    // right decimal is closer.
    { 1.3421778e+08f, {    +4,  8388611 }, {    +1, 13421778 }, __LINE__ },
    { 2.0971519e+06f, {    -3, 16777215 }, {    -1, 20971519 }, __LINE__ },

    // value is a tie, left decimal wins tiebreak.
    { 3.3554476e+07f, {    +2,  8388619 }, {    +0, 33554476 }, __LINE__ },
    { 4.1943032e+06f, {    -2, 16777213 }, {    -1, 41943032 }, __LINE__ },

    // value is a tie, right decimal wins tiebreak.
    {  4.1943038e+06, {    -2, 16777215 }, {    -1, 41943038 }, __LINE__ },

    //--------------------------------------------------------------------------
    // Value is uncentred. Tests whether it can return shortest.
    //--------------------------------------------------------------------------

    // a < b, a or b can be a tie, s == b, b isn't a tie.
    {  1.717987e+10f, {   +11,  8388608 }, {    +4,  1717987 }, __LINE__ },

    // a < b, a or b can be a tie, s == b, b is a tie.
    // ???

    // a < b, a or b can be a tie, a < s < b.
    {  5.368709e+08f, {    +6,  8388608 }, {    +2,  5368709 }, __LINE__ },

    // a < b, a or b can be a tie, s == a.
    // ???

    // a < b, a or b can be a tie, s < a.
    { 1.3421773e+08f, {    +4,  8388608 }, {    +1, 13421773 }, __LINE__ },

    // a < b, neither a nor b can be a tie, s > a.
    {  6.044629e+23f, {   +56,  8388608 }, {   +17,  6044629 }, __LINE__ },
    {         5e-01f, {   -24,  8388608 }, {    -1,        5 }, __LINE__ },

    //--------------------------------------------------------------------------
    // Value is uncentred. Return closest.
    //--------------------------------------------------------------------------

    // a < b, c != a, value is exact.
    {  9.765625e-04f, {   -33,  8388608 }, {   -10,  9765625 }, __LINE__ },

    // a < b, c != a, left decimal is closer.
    { 1.0737418e+09f, {    +7,  8388608 }, {    +2, 10737418 }, __LINE__ },
    { 1.1805916e+21f, {   +47,  8388608 }, {   +14, 11805916 }, __LINE__ },
    { 1.2207031e-04f, {   -36,  8388608 }, {   -11, 12207031 }, __LINE__ },

    // a < b, c != a, right decimal is closer.
    { 2.6843546e+08f, {    +5,  8388608 }, {    +1, 26843546 }, __LINE__ },
    { 3.8146973e-06f, {   -41,  8388608 }, {   -13, 38146973 }, __LINE__ },

    // a < b, c != a, value is a tie, left decimal wins tiebreak.
    { 1.6777216e+07f, {    +1,  8388608 }, {    +0, 16777216 }, __LINE__ },
    { 2.4414062e-04f, {   -35,  8388608 }, {   -11, 24414062 }, __LINE__ },

    // a < b, c != a, value is a tie, right decimal wins tiebreak.
    // ???

    // a < b, c == a, a is a tie.
    // ???

    // a < b, c == a, a isn't a tie.
    {  2.3841858e-07, {   -45,  8388608 }, {   -14,  23841858 }, __LINE__ },

    // a == b, a isn't a tie, left decimal is closer.
    { 9.9035203e+27f, {   +70,  8388608 }, {   +20, 99035203 }, __LINE__ },
    { 9.8607613e-32f, {  -126,  8388608 }, {   -39, 98607613 }, __LINE__ },

    // a == b, a isn't a tie, right decimal is closer.
    { 8.6736174e-19f, {   -83,  8388608 }, {   -26, 86736174 }, __LINE__ },
  };

  char chars[100]{};

  for (auto const [value, binary, decimal, line] : data) {

    auto const binary_actual  = traits_t::to_binary(value);
    auto const decimal_actual = traits_t::teju(value);

    if (binary_actual != binary || decimal_actual != decimal) {
      value_to_chars(chars, value);
      ASSERT_EQ(binary_actual, binary) <<
        "    Value = " << chars << " ~= " << decimal << "\n"
        "    Note: test case line = " << line;
      ASSERT_EQ(decimal_actual, decimal) <<
        "    Value = " << chars << " ~= " << binary  << "\n"
        "    Note: test case line = " << line;
    }
  }
}

template <typename /*TFloat*/>
class typed_tests_t : public testing::Test {
};

TYPED_TEST_SUITE_P(typed_tests_t);

// Test results for the minimum mantissa and all exponents. This test is
// parameterized on the floating-point number type and is instantiated for float
// and double.
TYPED_TEST_P(typed_tests_t, mantissa_min_all_exponents) {

  using float_t  = TypeParam;
  using traits_t = teju::traits_t<float_t>;
  using u1_t     = typename traits_t::u1_t;

  for (std::uint32_t exponent = traits_t::exponent_min; !this->HasFailure() &&
    exponent <= traits_t::exponent_max; ++exponent) {

    auto const bits = teju_pow2(u1_t, traits_t::mantissa_width - 1);
    float_t value;
    std::memcpy(&value, &bits, sizeof(bits));
    compare_to_others(value);
  }
}

TYPED_TEST_P(typed_tests_t, integers) {

  using float_t = TypeParam;

  auto test = [this](float_t const min, float_t const max) {
    for (float_t value = min; value < max && !this->HasFailure(); ++value) {
      compare_to_others(value);
      ASSERT_NE(value + 1.0, value);
    }
  };

  auto const max   = std::pow(float_t{2},
    float_t{traits_t<float_t>::mantissa_width - 1});
  auto const limit = float_t{10'000'000};

  if (max <= limit) {
    // Tests all small integers.
    test(float_t{1}, max);
  }
  else {
    // Tests first small integers.
    test(float_t{1}, limit);
    // Tests last  small integers.
    test(std::max(limit, max - limit), max);
  }
}

REGISTER_TYPED_TEST_SUITE_P(typed_tests_t,
  mantissa_min_all_exponents,
  integers
);

using float_types_t = ::testing::Types<float, double>;
INSTANTIATE_TYPED_TEST_SUITE_P(typed_tests_t, typed_tests_t, float_types_t);

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
