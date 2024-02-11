#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "amaru/float128.h"
#include "amaru/literal.h"
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"
#include "cpp/common/other.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>

namespace {

using namespace amaru;
using namespace amaru::test;

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
 * @brief Converts a given value from binary to decimal using Amaru (different
 * methods) and a third part-library and check whether they match.
 *
 * @tparam T                The floating point value type.
 * @param  value            The given value.
 */
template <typename T>
void compare_to_other(T const value) {

  using      traits_t = amaru::traits_t<T>;
  auto const amaru    = traits_t::amaru(value);
  auto const other    = traits_t::dragonbox_full(value);
  auto const fields   = traits_t::value_to_ieee(value);

  EXPECT_EQ(other, amaru) << "IEEE fields: " << fields;
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_other) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<std::int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = traits_t<float>::value_to_ieee(value);
    if (ieee.c.exponent != exponent) {
      exponent = ieee.c.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

    compare_to_other(value);

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

  using traits_t = amaru::traits_t<TypeParam>;
  using fp_t     = TypeParam;
  using u1_t     = typename traits_t::u1_t;

  auto const exponent_max = (std::uint32_t{1} << traits_t::exponent_size) - 1;

  for (std::uint32_t exponent = 1; !this->HasFailure() &&
    exponent < exponent_max; ++exponent) {

    auto const bits = u1_t{exponent} << traits_t::mantissa_size;
    fp_t value;
    std::memcpy(&value, &bits, sizeof(bits));
    compare_to_other(value);
  }
}

TYPED_TEST_P(typed_tests_t, small_integers) {

  using fp_t = TypeParam;

  // Let [1, M<T>[ be the interval of T-values for which the optimisation for
  // small integers applies. The tested range is [1, min(M<T>, M<float>)[.

  auto constexpr min = fp_t{1};
  auto constexpr exp = std::min(traits_t<fp_t>::mantissa_size,
    traits_t<float>::mantissa_size);
  auto const     max = std::pow(fp_t{2.0}, exp + 1);

  for (fp_t value = min; value < max && !this->HasFailure(); ++value) {
    compare_to_other(value);
    ASSERT_NE(value + 1.0, value);
  }
}

REGISTER_TYPED_TEST_SUITE_P(typed_tests_t,
  mantissa_min_all_exponents,
  small_integers
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
    compare_to_other(value);
  }
}

#if defined(AMARU_HAS_FLOAT128)

template <char... Cs>
uint128_t operator ""_u128() {
  return make_number<uint128_t, Cs...>::value(0);
}

TEST(float128, test_hard_coded_values) {

  using traits_t    = amaru::traits_t<float128_t>;
  using fields_t    = cpp_fields_t<float128_t>;
  using test_case_t = amaru::test::test_case_t<float128_t>;

  static auto constexpr amaru_size = std::uint32_t{128};

  struct test_data_t {
    float128_t value;
    fields_t   decimal;
  };

  test_data_t data[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    //             value    exponent                                mantissa
    {          1.0000000,          0,                                      1 },
    {          2.0000000,          0,                                      2 },
    {          3.0000000,          0,                                      3 },
    {          4.0000000,          0,                                      4 },
    {          5.0000000,          0,                                      5 },
    {          6.0000000,          0,                                      6 },
    {          7.0000000,          0,                                      7 },
    {          8.0000000,          0,                                      8 },
    {          9.0000000,          0,                                      9 },
    {         10.0000000,          1,                                      1 },
    {         11.0000000,          0,                                     11 },
    {         20.0000000,          1,                                      2 },
    {        100.0000000,          2,                                      1 },
    {       1000.0000000,          3,                                      1 },
    {      10000.0000000,          4,                                      1 },
    {     100000.0000000,          5,                                      1 },
    {    1000000.0000000,          6,                                      1 },
    {   10000000.0000000,          7,                                      1 },
    {  100000000.0000000,          8,                                      1 },
    { 1000000000.0000000,          9,                                      1 },

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    //             value    exponent                                mantissa
    {          0.5000000,         -1,                                      5 },
    {          0.2500000,         -2,                                     25 },
    {          0.1250000,         -3,                                    125 },
    {          0.7500000,         -2,                                     75 },

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    //             value    exponent                                 mantissa
    {          0.3000000,        -34, 2999999999999999888977697537484346_u128 },
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const value     = data[i].value;
    auto const test_case = test_case_t{value, data[i].decimal};
    auto const actual    = traits_t::amaru(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case number = " << i;
  }
}

TEST(float128, test_hard_coded_binary_representations) {

  using traits_t    = amaru::traits_t<float128_t>;
  using fields_t    = traits_t::fields_t;
  using test_case_t = amaru::test::test_case_t<float128_t>;

  static auto constexpr amaru_size = std::uint32_t{128};

  struct test_data_t {
    fields_t binary;
    fields_t decimal;
  };

  test_data_t data[] = {
    // Binary                                                Decimal
    // exponent                                 mantissa     exponent                                 mantissa
    {{     -114, 6230756230241792923652294673694720_u128 }, {     -34, 2999999999999999888977697537484346_u128 }},
  };

  for (std::size_t i = 0; i < std::size(data); ++i) {
    auto const test_case = test_case_t{data[i].binary, data[i].decimal};
    auto const actual    = traits_t::amaru(test_case.value());
    ASSERT_EQ(test_case.expected(), actual) <<
      "    Note: test case number = " << i;
  }
}

#endif // defined(AMARU_HAS_FLOAT128)

/**
 * @brief Returns the floating point number value corresponding to given IEEE
 * fields.
 *
 * @tparam T                The floating point value type.
 * @param  e                The given exponent field.
 * @param  m                The given mantissa field.
 */
template <typename T>
T
from_ieee(std::uint32_t e, typename traits_t<T>::u1_t m) {

  using      traits_t = amaru::traits_t<T>;
  using      u1_t     = typename traits_t::u1_t;
  u1_t const i        = (u1_t(e) << traits_t::mantissa_size) | m;

  T value;
  std::memcpy(&value, &i, sizeof(i));

  return value;
}

// Adhoc test for a given floating point number value.
TEST(ad_hoc, value) {
  auto const value = 1.0f;
  compare_to_other(value);
}

// Adhoc test for given field values.
TEST(ad_hoc, fields) {
  auto const value = from_ieee<float>(127, 0); // = 1.0f
  compare_to_other(value);
}

} // namespace <anonymous>
