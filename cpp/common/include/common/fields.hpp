// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file common/fields.hpp
 *
 * C++ wrapper around the C fields of floating-point representation.
 */

#ifndef TEJU_CPP_COMMON_INCLUDE_COMMON_FIELDS_HPP_
#define TEJU_CPP_COMMON_INCLUDE_COMMON_FIELDS_HPP_

#include "teju/src/config.h"

#if defined(teju_has_float128)
  #include <boost/multiprecision/cpp_int.hpp>
#endif

#include <iosfwd>

namespace teju {

/**
 * @brief C++ wrapper around the C fields of floating-point representation.
 *
 * Instantiations are provided for float, double and float128_t (if supported).
 */
template <typename /*TFloat*/>
struct cpp_fields_t;

#if defined(teju_has_float16)
template <>
struct cpp_fields_t<float16_t> : teju32_fields_t {
  using streamable_t = std::uint32_t;
};
#endif // defined(teju_has_float16)

template <>
struct cpp_fields_t<float> : teju32_fields_t {
  using streamable_t = std::uint32_t;
};

template <>
struct cpp_fields_t<double> : teju64_fields_t {
  using streamable_t = std::uint64_t;
};

#if defined(teju_has_float128)
template <>
struct cpp_fields_t<float128_t> : teju128_fields_t {
  using streamable_t = boost::multiprecision::uint128_t;
};
#endif // defined(teju_has_float128)

template <typename TFloat, unsigned base>
struct fields_t : cpp_fields_t<TFloat> {
};

template <typename TFloat>
using binary_t = fields_t<TFloat, 2>;

template <typename TFloat>
using decimal_t = fields_t<TFloat, 10>;

/**
 * @brief Equality operator for cpp_fields_t<TFloat>.
 *
 * @tparam TFloat           The type TFloat.
 *
 * @param  left             The left operand.
 * @param  right            The right operand.
 */
template <typename TFloat, unsigned base>
bool operator==(fields_t<TFloat, base> const& left,
  fields_t<TFloat, base> const& right) {
  return left.exponent == right.exponent && left.mantissa == right.mantissa;
}

/**
 * @brief Inequality operator for cpp_fields_t<TFloat>.
 *
 * @tparam TFloat           The type TFloat.
 *
 * @param  left             The left operand.
 * @param  right            The right operand.
 */
template <typename TFloat, unsigned base>
bool operator!=(fields_t<TFloat, base> const& left,
  fields_t<TFloat, base> const& right) {
  return !(left == right);
}

/**
 * @brief Stream operator for cpp_fields_t<TFloat>.
 *
 * @tparam C                The char type.
 * @tparam CT               The char-traits type.
 * @tparam TFloat           The type TFloat.
 *
 * @param  os               The object that fields are streamed to.
 * @param  fields           The fields.
 */
template <typename C, typename CT, typename TFloat, unsigned base>
std::basic_ostream<C, CT>&
operator <<(std::basic_ostream<C, CT>& os,
  fields_t<TFloat, base> const& fields) {
  using streamable_t = typename cpp_fields_t<TFloat>::streamable_t;
  auto const mantissa = streamable_t{fields.mantissa};
  return os << mantissa << " * " << base << "^(" << fields.exponent << ")";
}

} // namespace teju

# endif // TEJU_CPP_COMMON_INCLUDE_COMMON_FIELDS_HPP_
