// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

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
#include "teju/ieee754.h"

#include "cpp/common/fields.hpp"
#include "cpp/common/dragonbox.hpp"

namespace teju {

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
 * \li u1_t               The 1-limb unsigned integer type.
 * \li fields_t           Fields type storing exponent and mantissa.
 * \li streamable_float_t A type constructible from the one of interest that is
 *                        streamable.
 * \li streamable_uint_t  A type constructible from u1_t that is streamable.
 *
 * Static data:
 *
 * \li exponent_size Exponent size in bits.
 * \li mantissa_size Mantissa size in bits.
 *
 * Static functions:
 *
 * \li fields_t to_ieee(T value)
 *   Returns IEEE-754's representation of value.
 * \li fields_t teju(T value)
 *   Returns Teju Jagua's decimal fields of value.
 * \li fields_t other(T value)
 *   Returns the third-party library's binary fields of value.
 *
 * \tparam T                The type of interest.
 */
template <typename T>
struct traits_t;

namespace detail {

  /**
   * @brief Returns the IEEE-754 floating point value corresponding to the
   * given mantissa and exponent.
   *
   * \tparam T              The floating point number type.
   * \tparam mantissa_size  The number of bits in the mantissa.
   * \tparam U              An unsigned integer type of the same size as T.
   */
  template <typename T, std::uint32_t mantissa_size>
  T
  ieee_to_value(typename teju::traits_t<T>::fields_t ieee) {

    using traits_t = teju::traits_t<T>;
    using u1_t     = typename traits_t::u1_t;

    static_assert(sizeof(T) == sizeof(ieee.mantissa), "Incompatible types");

    auto const exponent = static_cast<u1_t>(ieee.exponent);
    auto const bits     = (exponent << mantissa_size) | ieee.mantissa;

    T value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
  }

  /**
   * @brief Converts IEEE-754 parameters to Teju Jagua's.
   *
   * \tparam exponent_size_ IEEE-754's exponent size.
   * \tparam mantissa_size_ IEEE-754's mantissa size.
   * \tparam exponent_min_  IEEE-754's exponent minimum.
   * \tparam exponent_max_  IEEE-754's exponent maximum.
   */
  template <
    std::int32_t exponent_size_,
    std::int32_t mantissa_size_,
    std::int32_t exponent_min_,
    std::int32_t exponent_max_
  >
  struct teju_from_ieee754_t {

    static auto constexpr exponent_size = std::uint32_t{exponent_size_};
    static auto constexpr mantissa_size = std::uint32_t{mantissa_size_};
    static auto constexpr exponent_min  = exponent_min_ - mantissa_size_;
    static auto constexpr exponent_max  = exponent_max_ - mantissa_size_;

  };

} // namespace detail

// Specialisation of traits_t for float.
template <>
struct traits_t<float> : detail::teju_from_ieee754_t<
  teju_ieee754_binary32_exponent_size,
  teju_ieee754_binary32_mantissa_size,
  teju_ieee754_binary32_exponent_min,
  teju_ieee754_binary32_exponent_max
  > {

  using u1_t     = teju32_u1_t;
  using fields_t = cpp_fields_t<float>;

  static
  fields_t
  value_to_ieee(float const value) {
    return fields_t{teju_float_to_ieee32(value)};
  }

  static
  float
  ieee_to_value(fields_t ieee) {
    return detail::ieee_to_value<float, mantissa_size>(ieee);
  }

  static
  fields_t
  ieee_to_binary(fields_t ieee32) {
    return fields_t{teju_ieee32_to_binary(ieee32)};
  }

  static
  fields_t
  teju(float const value) {
    return fields_t{teju_float_to_decimal(value)};
  }

  static
  fields_t
  dragonbox(float const value) {
    auto const fields = teju::dragonbox::to_decimal(value);
    return fields_t{std::int32_t{fields.exponent}, u1_t{fields.significand}};
  }

}; // traits_t<float>

// Specialisation of traits_t for float.
template <>
struct traits_t<double> : detail::teju_from_ieee754_t<
  teju_ieee754_binary64_exponent_size,
  teju_ieee754_binary64_mantissa_size,
  teju_ieee754_binary64_exponent_min,
  teju_ieee754_binary64_exponent_max
  > {

  using u1_t     = teju64_u1_t;
  using fields_t = cpp_fields_t<double>;

  static
  fields_t
  value_to_ieee(double const value) {
    return fields_t{teju_double_to_ieee64(value)};
  }

  static
  double
  ieee_to_value(fields_t ieee) {
    return detail::ieee_to_value<double, mantissa_size>(ieee);
  }

  static
  fields_t
  ieee_to_binary(fields_t ieee64) {
    return fields_t{teju_ieee64_to_binary(ieee64)};
  }

  static
  fields_t
  teju(double const value) {
    return fields_t{teju_double_to_decimal(value)};
  }

  static
  fields_t
  dragonbox(double const value) {
    auto const fields = teju::dragonbox::to_decimal(value);
    return fields_t{std::int32_t{fields.exponent}, u1_t{fields.significand}};
  }

}; // traits_t<double>

#if defined(teju_has_float128)

// Specialisation of traits_t for float128.
template <>
struct traits_t<float128_t> : detail::teju_from_ieee754_t<
  teju_ieee754_binary128_exponent_size,
  teju_ieee754_binary128_mantissa_size,
  teju_ieee754_binary128_exponent_min,
  teju_ieee754_binary128_exponent_max> {

  using u1_t     = teju128_u1_t;
  using fields_t = cpp_fields_t<float128_t>;

  static
  fields_t
  value_to_ieee(float128_t const value) {
    return fields_t{teju_float128_to_ieee128(value)};
  }

  static
  float128_t
  ieee_to_value(fields_t ieee) {
    return detail::ieee_to_value<float128_t, mantissa_size>(ieee);
  }

  static
  fields_t
  ieee_to_binary(fields_t ieee128) {
    return fields_t{teju_ieee128_to_binary(ieee128)};
  }

  static
  fields_t
  teju(float128_t const value) {
    return {teju_float128_to_decimal(value)};
  }

  // TODO (CN): Perhaps we could use Ryu for float128_t but at the moment
  // testing and benchmarking against other libraries is not supported.

}; // traits_t<float128_t>

#endif // defined(teju_has_float128)

} // namespace teju

#endif // TEJU_CPP_COMMON_TRAITS_HPP_
