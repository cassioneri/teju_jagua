
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"

#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
#include "amaru/float128.h"
#include "amaru/pack.h"
#include "other/other.hpp"

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
 * @brief Gets the next value of type \e T following a given one.
 *
 * @pre <tt>value >= 0</tt>.
 *
 * @tparam T                The floating point value type.
 * @param  value            The given value.
 */
template <typename T>
T
get_next(T value) {
  typename fp_traits_t<T>::u1_t i;
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

  using          traits_t     = fp_traits_t<T>;

  auto constexpr digits       = std::numeric_limits<T>::digits10 + 2;
  auto const     compact      = traits_t::amaru_compact(value);
  auto const     full         = traits_t::amaru_full(value);
  auto const     other        = traits_t::dragonbox_full(value);

  auto const     test_compact = test_case_t<T>{value, compact, other};
  auto const     test_full    = test_case_t<T>{value, full   , other};

  EXPECT_TRUE(test_compact) << test_compact;
  EXPECT_TRUE(test_full   ) << test_full;
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_other) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<std::int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = fp_traits_t<float>::value_to_ieee(value);
    if (ieee.exponent != exponent) {
      exponent = ieee.exponent;
      std::cerr << "Exponent: " << exponent << std::endl;
    }

    compare_to_other(value);

    value = get_next(value);
  }
}

template <typename T>
class TypedTests : public testing::Test {
};

TYPED_TEST_SUITE_P(TypedTests);

// Test results for the minimum mantissa and all exponents. This test is
// parameterized on the floating point number type and is instantiated for float
// and double.
TYPED_TEST_P(TypedTests, mantissa_min_all_exponents) {

  using traits_t          = fp_traits_t<TypeParam>;
  using fp_t              = TypeParam;
  using u1_t              = typename traits_t::u1_t;

  auto const exponent_max = (std::uint32_t{1} << traits_t::exponent_size) - 1;

  for (std::uint32_t exponent = 1; !this->HasFailure() &&
    exponent < exponent_max; ++exponent) {

    auto const bits = u1_t{exponent} << traits_t::mantissa_size;
    fp_t value;
    std::memcpy(&value, &bits, sizeof(bits));
    compare_to_other(value);
  }
}

REGISTER_TYPED_TEST_SUITE_P(TypedTests, mantissa_min_all_exponents);
using FpTypes = ::testing::Types<float, double>;
INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, TypedTests, FpTypes);

// Test results for a large number of random double values.
TEST(double, random_comparison_to_other) {

  using traits_t = fp_traits_t<double>;

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

TEST(float128, test_hard_coded_values) {

  using traits_t   = fp_traits_t<float128_t>;
  using fields_t   = traits_t::fields_t;

  static auto constexpr amaru_size = std::uint32_t{128};

  struct test_case_t {
    float128_t value;
    fields_t   expected;
  };

  auto f = [](uint64_t u, uint64_t l) {
    auto const p10_to_18 = uint128_t{1000000000000000000};
    return u * p10_to_18 + l;
  };

  test_case_t test_cases[] = {

    // -------------------------------------------------------------------------
    // Integer values
    // -------------------------------------------------------------------------

    // value                exponent                                mantissa
    {          1.0000000, {        0,                                      1 }},
    {          2.0000000, {        0,                                      2 }},
    {          3.0000000, {        0,                                      3 }},
    {          4.0000000, {        0,                                      4 }},
    {          5.0000000, {        0,                                      5 }},
    {          6.0000000, {        0,                                      6 }},
    {          7.0000000, {        0,                                      7 }},
    {          8.0000000, {        0,                                      8 }},
    {          9.0000000, {        0,                                      9 }},
    {         10.0000000, {        1,                                      1 }},
    {         11.0000000, {        0,                                     11 }},
    {         20.0000000, {        1,                                      2 }},
    {        100.0000000, {        2,                                      1 }},
    {       1000.0000000, {        3,                                      1 }},
    {      10000.0000000, {        4,                                      1 }},
    {     100000.0000000, {        5,                                      1 }},
    {    1000000.0000000, {        6,                                      1 }},
    {   10000000.0000000, {        7,                                      1 }},
    {  100000000.0000000, {        8,                                      1 }},
    { 1000000000.0000000, {        9,                                      1 }},

    // -------------------------------------------------------------------------
    // Perfectly represented fractional values
    // -------------------------------------------------------------------------

    // value                exponent                                mantissa
    {          0.5000000, {       -1,                                      5 }},
    {          0.2500000, {       -2,                                     25 }},
    {          0.1250000, {       -3,                                    125 }},
    {          0.7500000, {       -2,                                     75 }},

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------

    // value                exponent                                mantissa
    {          0.3000000, {      -34, f(2999999999999999, 888977697537484346)}},
  };

  for (unsigned i = 0; i < std::size(test_cases); ++i) {

    auto const value         = test_cases[i].value;
    auto const expected      = test_cases[i].expected;

    auto const amaru_compact = traits_t::amaru_compact(value);
    auto const amaru_full    = traits_t::amaru_full(value);

    EXPECT_EQ(expected.exponent, amaru_compact.exponent) <<
      "Note: test case number = " << i;

    EXPECT_EQ(expected.mantissa, amaru_compact.mantissa) <<
      "Note: test case number = " << i;
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
from_ieee(std::uint32_t e, typename fp_traits_t<T>::u1_t m) {

  using      traits_t = fp_traits_t<T>;
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
