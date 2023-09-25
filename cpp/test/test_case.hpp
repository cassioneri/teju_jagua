#ifndef AMARU_CPP_TEST_TEST_CASE_HPP_
#define AMARU_CPP_TEST_TEST_CASE_HPP_

#include "amaru/config.h"
#include "cpp/common/fields.hpp"
#include "cpp/common/traits.hpp"

#include <iosfwd>

namespace amaru {
namespace test {

/**
 * @brief Test case for a given type.
 * 
 * It stores a value of type \e T and the expected fields of its Amaru's
 * decimal representation. Instantiations are provided for \c float, \c double
 * and \c float128_t (if supported).
 * 
 * \tparam                  The given type.
 */
template <typename T>
struct test_case_t {

  using fields_t = cpp_fields_t<T>;

  /**
   * @brief Constructor from a value and the expected fields of its Amaru's
   * decimal representation.
   * 
   * @param value           The given value.
   * @param expected        The expected fields of Amaru's decimal
   *                        representation.
   */
  explicit test_case_t(T value, fields_t const& expected);

  /**
   * @brief Constructor from the fields of Amaru's binary representation and the
   * expected fields of its Amaru's decimal representation.
   * 
   * @param value           The fields of Amaru's binary representation.
   * @param expected        The expected fields of Amaru's decimal
   *                        representation.
   */
  explicit test_case_t(fields_t const& amaru_binary, fields_t const& expected);

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
 * @brief Stream operator for <tt>test_case_t<T></tt>.
 * 
 * @tparam C                The char type.
 * @tparam CT               The char-traits type.
 * @tparam T                The type \e T.
 * 
 * @param os                The object that the test case is streamed to.
 * @param test_case         The test case.
 */
template <typename C, typename CT, typename T>
std::basic_ostream<C, CT>&
operator <<(std::basic_ostream<C, CT>& os, test_case_t<T> const& test_case) {

  using traits_t          = amaru::traits_t<T>;
  using fields_t          = typename test_case_t<T>::fields_t;
  using streamable_uint_t = typename fields_t::streamable_uint_t;

  auto const value        = test_case.value();
  auto const ieee         = traits_t::value_to_ieee(value);
  auto const amaru_binary = traits_t::ieee_to_amaru_binary(ieee);

  // Specially formatted for looking pretty in gtest messages.
  return os << test_case.expected() << "\n"
    "    [value = " << streamable_uint_t{amaru_binary.c.mantissa} <<
    " * 2^(" << amaru_binary.c.exponent << ")]";
}

} // namespace test
} // namespace amaru

#endif // AMARU_CPP_TEST_TEST_CASE_HPP_
