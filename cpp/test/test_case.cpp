#include "cpp/common/fp_traits.hpp"
#include "cpp/test/test_case.hpp"

#include <iostream>
#include <type_traits>

namespace amaru {
namespace test {

namespace {

/**
 * @brief Returns the floating point number value corresponding to given IEEE
 * fields.
 *
 * @tparam T                The floating point value type.
 * @param  ieee             The IEEE-754 representation of the floating point
 *                          number.
 */
template <typename T>
T
from_ieee(typename fp_traits_t<T>::fields_t const& ieee) {

  using      traits_t = fp_traits_t<T>;
  using      u1_t     = typename traits_t::u1_t;

  u1_t const i = (static_cast<u1_t>(ieee.exponent) << traits_t::mantissa_size) |
    ieee.mantissa;

  T value;
  std::memcpy(&value, &i, sizeof(i));

  return value;
}

} // namespace <anonymous>

template <typename T>
test_case_t<T>::test_case_t(T value, fields_t const& expected,
  fields_t const& actual) :
  value_   {value   },
  actual_  {actual  },
  expected_{expected} {
}

template <typename T>
test_case_t<T>::test_case_t(fields_t const& ieee,
  fields_t const& expected, fields_t const& actual) :
  test_case_t{from_ieee<T>(ieee), expected, actual} {
}

template <typename T>
test_case_t<T>::operator bool() const {
  return actual_.exponent == expected_.exponent &&
    actual_.mantissa == expected_.mantissa;
};

template <typename T>
std::ostream& operator <<(std::ostream& os, test_case_t<T> const& test_case) {

  using traits_t   = fp_traits_t<T>;
  using uint_t     = typename traits_t::streamable_uint_t;
  auto const value = test_case.value_;
  auto const ieee  = traits_t::to_ieee(value);

  return os <<
    "  exponent:\n"
    "    ieee            = " << ieee.exponent                        << "\n"
    "    actual          = " << test_case.expected_.exponent         << "\n"
    "    expected        = " << test_case.actual_  .exponent         << "\n"
    "  mantissa:\n"
    "    ieee            = " << uint_t{ieee.mantissa}                << "\n"
    "    actual          = " << uint_t{test_case.actual_  .mantissa} << "\n"
    "    expected        = " << uint_t{test_case.expected_.mantissa} << "\n";
}

template
struct test_case_t<float>;

template
struct test_case_t<double>;

template
struct test_case_t<float128_t>;

template
std::ostream&
operator <<(std::ostream&, test_case_t<float> const&);

template
std::ostream&
operator <<(std::ostream&, test_case_t<double> const&);

template
std::ostream&
operator <<(std::ostream&, test_case_t<float128_t> const&);

} // namespace test
} // namespace amaru
