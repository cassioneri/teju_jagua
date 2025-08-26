// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/test/test_case.hpp
 *
 * Implementation of test_case_t.
 */

#ifndef TEJU_CPP_TEST_TEST_CASE_HPP_
#define TEJU_CPP_TEST_TEST_CASE_HPP_

#include "common/fields.hpp"

namespace teju::test {

/**
 * @brief Test case for a given type.
 *
 * It stores a value of type T and the expected fields of its Tejú Jaguá's
 * decimal representation. Instantiations are provided for float, double and
 * float128_t (if supported).
 *
 * @tparam T                The given type.
 */
template <typename T>
struct test_case_t {

  /**
   * @brief Constructor from a value and the expected fields of its Tejú Jaguá's
   *        decimal representation.
   *
   * @param  value          The given value.
   * @param  expected       The expected fields of Tejú Jaguá's decimal
   *                        representation.
   */
  explicit test_case_t(T value, decimal_t<T> const& expected);

  /**
   * @brief Gets the value.
   *
   * @returns The value.
   */
  T const&
  value() const;

  /**
   * @brief Gets the expected representation.
   *
   * @returns The representation.
   */
  decimal_t<T> const&
  expected() const;

private:

  T            value_;
  decimal_t<T> expected_;

}; // test_case_t

} // namespace teju::test

#endif // TEJU_CPP_TEST_TEST_CASE_HPP_
