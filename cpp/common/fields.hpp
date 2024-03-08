// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/fields.hpp
 *
 * C++ wrapper around the C fields of floating point representation.
 */

#ifndef TEJU_CPP_COMMON_FIELDS_HPP_
#define TEJU_CPP_COMMON_FIELDS_HPP_

#if defined(teju_has_float128)
  #include <boost/multiprecision/cpp_int.hpp>
#endif

#include <iostream>

namespace teju {

/**
 * @brief C++ wrapper around the C fields of floating point representation.
 *
 * Instantiations are provided for float, double and float128_t (if supported).
 */
template <typename T>
struct cpp_fields_t;

template <>
struct cpp_fields_t<float> : teju32_fields_t {
  using streamable_uint_t = std::uint32_t;
};

template <>
struct cpp_fields_t<double> : teju64_fields_t {
  using streamable_uint_t = std::uint64_t;
};

#if defined(teju_has_float128)
template <>
struct cpp_fields_t<float128_t> : teju128_fields_t {
  using streamable_uint_t = boost::multiprecision::uint128_t;
};
#endif // defined(teju_has_float128)

/**
 * @brief Equality operator for cpp_fields_t<T>.
 *
 * @tparam T                The type T.
 *
 * @param left              The left operand.
 * @param right             The right operand.
 */
template <typename T>
bool operator==(cpp_fields_t<T> const& left, cpp_fields_t<T> const& right) {
  return left.exponent == right.exponent && left.mantissa == right.mantissa;
}

/**
 * @brief Inequality operator for cpp_fields_t<T>.
 *
 * @tparam T                The type T.
 *
 * @param left              The left operand.
 * @param right             The right operand.
 */
template <typename T>
bool operator!=(cpp_fields_t<T> const& left, cpp_fields_t<T> const& right) {
  return !(left == right);
}

/**
 * @brief Stream operator for cpp_fields_t<T>.
 *
 * @tparam C                The char type.
 * @tparam CT               The char-traits type.
 * @tparam T                The type T.
 *
 * @param os                The object that fields are streamed to.
 * @param fields            The fields.
 */
template <typename C, typename CT, typename T>
std::basic_ostream<C, CT>&
operator <<(std::basic_ostream<C, CT>& os, cpp_fields_t<T> const& fields) {

  using streamable_uint_t = typename cpp_fields_t<T>::streamable_uint_t;

  return os <<
    "exponent = " << fields.exponent                    << ", "
    "mantissa = " << streamable_uint_t{fields.mantissa};
}

} // namespace teju

# endif // TEJU_CPP_COMMON_FIELDS_HPP_
