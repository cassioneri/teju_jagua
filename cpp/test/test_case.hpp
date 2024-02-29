// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/test/test_case.hpp
 *
 * Implementation of test_case_t.
 */

#ifndef TEJU_CPP_TEST_TEST_CASE_HPP_
#define TEJU_CPP_TEST_TEST_CASE_HPP_

#include "teju/config.h"
#include "cpp/common/fields.hpp"
#include "cpp/common/traits.hpp"

#include <iosfwd>

namespace teju {
namespace test {

/**
 * @brief Test case for a given type.
 *
 * It stores a value of type T and the expected fields of its Teju Jagua's
 * decimal representation. Instantiations are provided for float, double and
 * float128_t (if supported).
 *
 * \tparam                  The given type.
 */
template <typename T>
struct test_case_t {

  using fields_t = cpp_fields_t<T>;

  /**
   * @brief Constructor from a value and the expected fields of its Teju Jagua's
   * decimal representation.
   *
   * @param value           The given value.
   * @param expected        The expected fields of Teju Jagua's decimal
   *                        representation.
   */
  explicit test_case_t(T value, fields_t const& expected);

  /**
   * @brief Constructor from the fields of Teju Jagua's binary representation
   * and the expected fields of its Teju Jagua's decimal representation.
   *
   * @param value           The fields of Teju Jagua's binary representation.
   * @param expected        The expected fields of Teju Jagua's decimal
   *                        representation.
   */
  explicit test_case_t(fields_t const& teju_binary, fields_t const& expected);

  /**
   * @brief Gets the value.
   *
   * @returns The value.
   */
  T const& value() const;

  /**
   * @brief Gets the expected representation.
   *
   * @returns The representation.
   */
  fields_t const& expected() const;

private:

  T        value_;
  fields_t expected_;

}; // test_case_t

/**
 * @brief Stream operator for test_case_t<T>.
 *
 * @tparam C                The char type.
 * @tparam CT               The char-traits type.
 * @tparam T                The type T.
 *
 * @param os                The object that the test case is streamed to.
 * @param test_case         The test case.
 */
template <typename C, typename CT, typename T>
std::basic_ostream<C, CT>&
operator <<(std::basic_ostream<C, CT>& os, test_case_t<T> const& test_case) {

  using traits_t          = teju::traits_t<T>;
  using fields_t          = typename test_case_t<T>::fields_t;
  using streamable_uint_t = typename fields_t::streamable_uint_t;

  auto const value        = test_case.value();
  auto const ieee         = traits_t::value_to_ieee(value);
  auto const teju_binary  = traits_t::ieee_to_teju_binary(ieee);

  // Specially formatted for looking pretty in gtest messages.
  return os << test_case.expected() << "\n"
    "    [value = " << streamable_uint_t{teju_binary.c.mantissa} << " * 2^(" <<
    teju_binary.c.exponent << ")]";
}

} // namespace test
} // namespace teju

#endif // TEJU_CPP_TEST_TEST_CASE_HPP_
