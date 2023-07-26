#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/double.h"
#include "amaru/float.h"
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

/**
 * @brief Traits for floating point number types.
 *
 * Amaru (and possibly other third-party libraries with the same purpose) are
 * supposed to be called in C which doesn't support overloads or generic
 * programming (templates). This class serves to wrap C functions (and data)
 * referring to specific types into a generic interface. For instance,
 * \c amaru_from_double_to_decimal_compact and
 * \c amaru_from_float_to_decimal_compact are two such functions that are
 * wrapped, respectively, by \c fp_traits_t<double>::amaru_compact and
 * \c fp_traits_t<float>::amaru_compact, make easier to call then in generic
 * tests. Specializations of \c fp_traits_t are provided for \c float and
 * \c double. They contain the following members.
 *
 * Types:
 *
 * \li u1_t           The 1-limb \c unsigned integer type.
 * \li amaru_fields_t Amaru fields type.
 * \li other_fields_t The third-party library fields type.
 *
 * Static data:
 *
 * \li exponent_size  Exponent size in bits.
 * \li mantissa_size  Mantissa size in bits.
 *
 * Static functions:
 *
 * \li amaru_fields_t fields(T value)
 *   Returns Amaru binary fields of \e value.
 * \li amaru_fields_t amaru_compact(T value)
 *   Returns Amaru decimal fields of \e value found by the compact table method.
 * \li amaru_fields_t amaru_full(T value)
 *   Returns Amaru decimal fields of \e value found by the full table method.
 * \li other_fields_t other(T value)
 *   Returns the third-party library binary fields of \e value.
 * \li std::int32_t exponent(other_fields_t fields)
 *   Extracts the exponent field from \e fields.
 * \li u1_t mantissa(other_fields_t fields)
 *   Extracts the mantissa field from \e fields.
 */
template <typename T>
struct fp_traits_t;

// Specialization of fp_traits_t for float.
template <>
struct fp_traits_t<float> {

  using u1_t           = amaru32_u1_t;
  using amaru_fields_t = amaru32_fields_t;
  using other_fields_t = amaru::dragonbox_full::result_float_t;

  static auto constexpr exponent_size = uint32_t{8};
  static auto constexpr mantissa_size = uint32_t{23};

  static
  amaru_fields_t
  fields(float const value) {
    return amaru_from_float_to_fields(value);
  }

  static
  amaru_fields_t
  amaru_compact(float const value) {
    return amaru_from_float_to_decimal_compact(value);
  }

  static
  amaru_fields_t
  amaru_full(float const value) {
    return amaru_from_float_to_decimal_full(value);
  }

  static
  other_fields_t
  other(float const value) {
    return amaru::dragonbox_full::to_decimal(value);
  }

  static
  std::int32_t
  exponent(other_fields_t fields) {
    return std::int32_t{fields.exponent};
  }

  static
  u1_t
  mantissa(other_fields_t fields) {
    return u1_t{fields.significand};
  }

};

// Specialization of fp_traits_t for float.
template <>
struct fp_traits_t<double> {

  using u1_t           = amaru64_u1_t;
  using amaru_fields_t = amaru64_fields_t;
  using other_fields_t = amaru::dragonbox_full::result_double_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};

  static
  amaru_fields_t
  fields(double const value) {
    return amaru_from_double_to_fields(value);
  }

  static
  amaru_fields_t
  amaru_compact(double const value) {
    return amaru_from_double_to_decimal_compact(value);
  }

  static
  amaru_fields_t
  amaru_full(double const value) {
    return amaru_from_double_to_decimal_full(value);
  }

  static
  other_fields_t
  other(double const value) {
    return amaru::dragonbox_full::to_decimal(value);
  }

  static
  std::int32_t
  exponent(other_fields_t fields) {
    return std::int32_t{fields.exponent};
  }

  static
  u1_t
  mantissa(other_fields_t fields) {
    return u1_t{fields.significand};
  }
};

/**
 * @brief Gets the next value of type \e T following a given one.
 *
 * @pre value >= 0.
 *
 * @tparam T     The floating point value type.
 * @param  value The given value.
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
 *  This function uses \c EXPECT_EQ for the comparisons.
 *
 * @tparam T     The floating point value type.
 * @param  value The given value.
 */
template <typename T>
void compare_to_other(T const value) {

  using          traits_t      = fp_traits_t<T>;
  auto constexpr digits        = std::numeric_limits<T>::digits10 + 2;
  auto const     amaru_compact = traits_t::amaru_compact(value);
  auto const     amaru_full    = traits_t::amaru_full(value);
  auto const     other         = traits_t::other(value);
  auto const     ieee          = traits_t::fields(value);

  EXPECT_EQ(traits_t::exponent(other), amaru_compact.exponent) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;

  EXPECT_EQ(traits_t::exponent(other), amaru_full.exponent) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;

  EXPECT_EQ(traits_t::mantissa(other), amaru_compact.mantissa) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;

  EXPECT_EQ(traits_t::mantissa(other), amaru_full.mantissa) << "Note: "
    "value = " << std::setprecision(digits) << value << ", "
    "ieee.exponent = " << ieee.exponent << ", "
    "ieee.mantissa = " << ieee.mantissa;
}

// Test results for all possible strictly positive finite float values.
TEST(float, exhaustive_comparison_to_other) {

  auto value    = std::numeric_limits<float>::denorm_min();
  auto exponent = std::numeric_limits<int32_t>::min();

  while (std::isfinite(value) && !HasFailure()) {

    auto const ieee = fp_traits_t<float>::fields(value);
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

  auto const exponent_max = (uint32_t{1} << traits_t::exponent_size) - 1;

  for (uint32_t exponent = 1; !this->HasFailure() &&
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

  auto number_of_tests = uint32_t{100000000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (!HasFailure() && number_of_tests --> 0) {
    auto const i = dist(rd);
    double value;
    std::memcpy(&value, &i, sizeof(i));
    compare_to_other(value);
  }
}

/**
 * @brief Returns the floating point number value corresponding to given IEEE
 * fields.
 *
 * @tparam T     The floating point value type.
 * @param  e     The given exponent field.
 * @param  m     The given mantissa field.
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
