/**
 * @file cpp/common/fields.hpp
 *
 * C++ wrapper around the C fields of floating point representation.
 */

#ifndef AMARU_CPP_COMMON_FIELDS_HPP_
#define AMARU_CPP_COMMON_FIELDS_HPP_

#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>

namespace amaru {

/**
 * @brief C++ wrapper around the C fields of floating point representation.
 * 
 * Instantiations are provided for float, double and float128_t (if supported).
 */
template <typename T>
struct cpp_fields_t;

template <>
struct cpp_fields_t<float> {
  using streamable_uint_t = std::uint32_t;
  amaru32_fields_t c;
};

template <>
struct cpp_fields_t<double> {
  using streamable_uint_t = std::uint64_t;
  amaru64_fields_t c;
};

#if defined(AMARU_HAS_FLOAT128)
template <>
struct cpp_fields_t<float128_t> {
  using streamable_uint_t = boost::multiprecision::uint128_t;
  amaru128_fields_t c;
};
#endif // defined(AMARU_HAS_FLOAT128)

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
  return left.c.exponent == right.c.exponent &&
    left.c.mantissa == right.c.mantissa;
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
    "exponent = " << fields.c.exponent                    << ", "
    "mantissa = " << streamable_uint_t{fields.c.mantissa};
}

} // namespace amaru

# endif // AMARU_CPP_COMMON_FIELDS_HPP_
