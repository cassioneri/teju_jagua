// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/traits.hpp
 *
 * Traits for floating-point number types.
 */

#ifndef TEJU_CPP_COMMON_TRAITS_HPP_
#define TEJU_CPP_COMMON_TRAITS_HPP_

#include "teju/config.h"
#include "teju/double.h"
#include "teju/float.h"
#include "teju/float128.h"
#include "teju/float16.h"

#include "cpp/common/fields.hpp"
#include "cpp/common/dragonbox.hpp"
#include "cpp/common/ryu.hpp"

namespace teju {

namespace detail {

  /**
   * @brief Gets the value represented by a given binary field representation.
   *
   * @tparam T              The floating-point type.
   * @param  binary         The given binary field representation.
   *
   * @returns The value represented by a given binary field representation.
   */
  template <typename T>
  T
  to_value(binary_t<T> binary);

} // namespace detail

/**
 * @brief Traits for floating-point number types.
 *
 * Tejú Jaguá (and possibly other third-party libraries with the same purpose)
 * are supposed to be called in C which doesn't support overloading and
 * templates. This class serves to wrap C functions (and data) referring to
 * specific types into a generic interface. For instance,
 * teju_double_to_decimal is wrapped by traits_t<double>::teju, making easier to
 * call it in generic tests. Specialisations are provided for float, double and
 * some other floating-point types. For float and double they contain the all
 * the following members. (Other types might not implement them all.)
 *
 * Types:
 *
 * @li u1_t                 The 1-limb unsigned integer type.
 * @li decimal_t            Type of decimal field representation.
 * @li binary_t             Type of binary field representation.
 *
 * Static data:
 *
 * @li mantissa_width       Mantissa width (number of bits).
 * @li exponent_min         Minimum exponent of the binary representation.
 * @li exponent_max         Maximum exponent of the binary representation.
 *
 * Static functions:
 *
 * @li binary_t to_binary(T value)
 *   Returns the binary field representation of value.
 * @li binary_t to_value(binary_t binary)
 *   Returns the value corresponding to the given binary field representation.
 * @li decimal_t teju(T value)
 *   Returns the decimal field representation obtained by Tejú Jaguá.
 * @li decimal_t dragonbox(T value)
 *   Returns the decimal field representation obtained by Dragonbox.
 * @li decimal_t ryu(T value)
 *   Returns the decimal field representation obtained by Ryu.
 *
 * @tparam T                The floating-point type.
 */
template <typename T>
struct traits_t;

#if defined(teju_has_float16)

// Specialisation of traits_t for float16_t.
template <>
struct traits_t<float16_t> {

  using u1_t      = teju16_u1_t;
  using decimal_t = teju::decimal_t<float16_t>;
  using binary_t  = teju::binary_t<float16_t>;

  static auto constexpr mantissa_width =  11u;
  static auto constexpr exponent_min   = -24;
  static auto constexpr exponent_max   =   5;

  static
  binary_t
  to_binary(float16_t const value) {
    auto const binary = teju_float16_to_binary(value);
    return {binary.exponent, binary.mantissa};
  }

  static
  decimal_t
  teju(float16_t const value) {
    auto const decimal = teju_float16_to_decimal(value);
    return {decimal.exponent, decimal.mantissa};
  }

}; // traits_t<float16_t>

#endif // defined(teju_has_float16)

// Specialisation of traits_t for float.
template <>
struct traits_t<float> {

  using u1_t      = teju32_u1_t;
  using decimal_t = teju::decimal_t<float>;
  using binary_t  = teju::binary_t<float>;

  static auto constexpr mantissa_width =  24u;
  static auto constexpr exponent_min  = -149;
  static auto constexpr exponent_max  =  104;

  static
  binary_t
  to_binary(float const value) {
    auto const binary = teju_float_to_binary(value);
    return binary_t{binary.exponent, binary.mantissa};
  }

  static
  float
  to_value(binary_t const binary) {
    return detail::to_value(binary);
  }

  static
  auto
  teju_raw(float const value) {
    return teju_float_to_decimal(value);
  }

  static
  decimal_t
  teju(float const value) {
    auto const decimal = teju_raw(value);
    return {decimal.exponent, decimal.mantissa};
  }

  static
  auto
  dragonbox_raw(float const value) {
    return teju::dragonbox::to_decimal(value);
  }

  static
  decimal_t
  dragonbox(float const value) {
    auto const decimal = dragonbox_raw(value);
    return {std::int32_t{decimal.exponent}, u1_t{decimal.significand}};
  }

  static
  auto
  ryu_raw(float const value) {
    return ryu_float_to_decimal(value);
  }

  static
  decimal_t
  ryu(float const value) {
    auto const decimal = ryu_raw(value);
    return {decimal.exponent, decimal.mantissa};
  }

}; // traits_t<float>

// Specialisation of traits_t for float.
template <>
struct traits_t<double> {

  using u1_t      = teju64_u1_t;
  using decimal_t = teju::decimal_t<double>;
  using binary_t  = teju::binary_t<double>;

  static auto constexpr mantissa_width =  53u;
  static auto constexpr exponent_min  = -1074;
  static auto constexpr exponent_max  =   971;

  static
  binary_t
  to_binary(double const value) {
    auto const binary = teju_double_to_binary(value);
    return binary_t{binary.exponent, binary.mantissa};
  }

  static
  double
  to_value(binary_t const binary) {
    return detail::to_value(binary);
  }

  static
  auto
  teju_raw(double const value) {
    return teju_double_to_decimal(value);
  }

  static
  decimal_t
  teju(double const value) {
    auto const decimal = teju_raw(value);
    return {decimal.exponent, decimal.mantissa};
  }

  static
  auto
  dragonbox_raw(double const value) {
    return teju::dragonbox::to_decimal(value);
  }

  static
  decimal_t
  dragonbox(double const value) {
    auto const decimal = dragonbox_raw(value);
    return {std::int32_t{decimal.exponent}, u1_t{decimal.significand}};
  }

  static
  auto
  ryu_raw(double const value) {
    return ryu_double_to_decimal(value);
  }

  static
  decimal_t
  ryu(double const value) {
    auto const decimal = ryu_raw(value);
    return {decimal.exponent, decimal.mantissa};
  }
}; // traits_t<double>

#if defined(teju_has_float128)

// Specialisation of traits_t for float128.
template <>
struct traits_t<float128_t> {

  using u1_t      = teju128_u1_t;
  using decimal_t = teju::decimal_t<float128_t>;
  using binary_t  = teju::binary_t<float128_t>;

  static auto constexpr mantissa_width =    113u;
  static auto constexpr exponent_min   = -16494;
  static auto constexpr exponent_max   =  16271;

  static
  binary_t
  to_binary(float128_t const value) {
    auto const binary = teju_float128_to_binary(value);
    return binary_t{binary.exponent, binary.mantissa};
  }

  static
  decimal_t
  teju(float128_t const value) {
    auto const decimal = teju_float128_to_decimal(value);
    return {decimal.exponent, decimal.mantissa};
  }

}; // traits_t<float128_t>

#endif // defined(teju_has_float128)

} // namespace teju

#endif // TEJU_CPP_COMMON_TRAITS_HPP_
