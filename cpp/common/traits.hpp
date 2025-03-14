// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/traits.hpp
 *
 * Traits for floating point number types.
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

  template <typename T>
  T
  to_value(binary_t<T> const binary);

} // namespace detail

/**
 * @brief Traits for floating point number types.
 *
 * Teju Jagua (and possibly other third-party libraries with the same purpose)
 * are supposed to be called in C which doesn't support overloading and
 * templates. This class serves to wrap C functions (and data) referring to
 * specific types into a generic interface. For instance,
 * teju_from_double_to_decimal is wrapped by traits_t<double>::teju, making
 * easier to call it in generic tests. Specialisations of traits_t are provided
 * for float and double and, if supported float128_t. They contain the following
 * members.
 *
 * Types:
 *
 * @li u1_t               The 1-limb unsigned integer type.
 * @li fields_t           Fields type storing exponent and mantissa.
 * @li streamable_float_t A type constructible from the one of interest that is
 *                        streamable.
 * @li streamable_uint_t  A type constructible from u1_t that is streamable.
 *
 * Static data:
 *
 * @li exponent_size Exponent size in bits.
 * @li mantissa_size Mantissa size in bits.
 *
 * Static functions:
 *
 * @li fields_t to_ieee(T value)
 *   Returns IEEE-754's representation of value.
 * @li fields_t teju(T value)
 *   Returns Teju Jagua's decimal fields of value.
 * @li fields_t other(T value)
 *   Returns the third-party library's binary fields of value.
 *
 * @tparam T                The type of interest.
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

  static auto constexpr mantissa_size =  11u;
  static auto constexpr exponent_min  = -24;
  static auto constexpr exponent_max  =   5;

  static
  binary_t
  to_binary(float16_t const value) {
    auto const binary = teju_float16_to_binary(value);
    return {binary.mantissa, binary.exponent};
  }

  static
  decimal_t
  teju(float16_t const value) {
    auto const decimal = teju_float16_to_decimal(value);
    return {decimal.mantissa, decimal.exponent};
  }

}; // traits_t<float16_t>

#endif // defined(teju_has_float16)

// Specialisation of traits_t for float.
template <>
struct traits_t<float> {

  using u1_t      = teju32_u1_t;
  using decimal_t = teju::decimal_t<float>;
  using binary_t  = teju::binary_t<float>;

  static auto constexpr mantissa_size =  24u;
  static auto constexpr exponent_min = -149;
  static auto constexpr exponent_max =  104;

  static
  binary_t
  to_binary(float const value) {
    auto const binary = teju_float_to_binary(value);
    return binary_t{ binary.mantissa, binary.exponent };
  }

  static
  float
  to_value(binary_t const binary) {
    return detail::to_value(binary);
  }

  static
  decimal_t
  teju(float const value) {
    auto const decimal = teju_float_to_decimal(value);
    return {decimal.mantissa, decimal.exponent};
  }

  static
  decimal_t
  dragonbox(float const value) {
    auto const decimal = teju::dragonbox::to_decimal(value);
    return {u1_t{decimal.significand}, std::int32_t{decimal.exponent}};
  }

  static
  decimal_t
  ryu(float const value) {
    auto const decimal = ryu_float_to_decimal(value);
    return {decimal.mantissa, decimal.exponent};
  }

}; // traits_t<float>

// Specialisation of traits_t for float.
template <>
struct traits_t<double> {

  using u1_t      = teju64_u1_t;
  using decimal_t = teju::decimal_t<double>;
  using binary_t  = teju::binary_t<double>;

  static auto constexpr mantissa_size =  53u;
  static auto constexpr exponent_min = -1074;
  static auto constexpr exponent_max =   971;

  static
  binary_t
  to_binary(double const value) {
    auto const binary = teju_double_to_binary(value);
    return binary_t{ binary.mantissa, binary.exponent };
  }

  static
  double
  to_value(binary_t const binary) {
    return detail::to_value(binary);
  }

  static
  decimal_t
  teju(double const value) {
    auto const decimal = teju_double_to_decimal(value);
    return {decimal.mantissa, decimal.exponent};
  }

  static
  decimal_t
  dragonbox(double const value) {
    auto const decimal = teju::dragonbox::to_decimal(value);
    return {u1_t{decimal.significand}, std::int32_t{decimal.exponent}};
  }

  static
  decimal_t
  ryu(double const value) {
      auto const decimal = ryu_double_to_decimal(value);
      return {decimal.mantissa, decimal.exponent};
    }
}; // traits_t<double>

#if defined(teju_has_float128)

// Specialisation of traits_t for float128.
template <>
struct traits_t<float128_t> {

  using u1_t      = teju128_u1_t;
  using decimal_t = teju::decimal_t<float128_t>;
  using binary_t  = teju::binary_t<float128_t>;

  static auto constexpr mantissa_size =    113u;
  static auto constexpr exponent_min  = -16494;
  static auto constexpr exponent_max  =  16271;

  static
  binary_t
  to_binary(float128_t const value) {
    auto const binary = teju_float128_to_binary(value);
    return binary_t{ binary.mantissa, binary.exponent };
  }

  static
  decimal_t
  teju(float128_t const value) {
    auto const decimal = teju_float128_to_decimal(value);
    return {decimal.mantissa, decimal.exponent};
  }

}; // traits_t<float128_t>

#endif // defined(teju_has_float128)

} // namespace teju

#endif // TEJU_CPP_COMMON_TRAITS_HPP_
