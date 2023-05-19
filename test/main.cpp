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

template <typename>
struct fp_traits_t;

template <typename T>
T
get_next(T value) {
  typename fp_traits_t<T>::u1_t i;
  std::memcpy(&i, &value, sizeof(value));
  ++i;
  std::memcpy(&value, &i, sizeof(value));
  return value;
}

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

TEST(ad_hoc, value) {
  auto const value = 1.0f;
  compare_to_other(value);
}

TEST(ad_hoc, fields) {
  auto const value = from_ieee<float>(127, 0); // = 1.0f
  compare_to_other(value);
}

} // namespace <anonymous>
