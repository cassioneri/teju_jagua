#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"

#include <iostream>
#include <type_traits>

namespace amaru {
namespace test {

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
  test_case_t{traits_t::ieee_to_value(ieee), expected, actual} {
}

template <typename T>
test_case_t<T>::operator bool() const {
  return actual_.exponent == expected_.exponent &&
    actual_.mantissa == expected_.mantissa;
};

template <typename T>
std::ostream& operator <<(std::ostream& os, test_case_t<T> const& test_case) {

  using traits_t          = amaru::traits_t<T>;
  using uint_t            = typename traits_t::streamable_uint_t;
  auto const value        = test_case.value_;
  auto const ieee         = traits_t::value_to_ieee(value);
  auto const amaru_binary = traits_t::ieee_to_amaru_binary(ieee);

  return os <<
    "  value             = " << uint_t{amaru_binary.mantissa}        <<
      " * 2^(" << amaru_binary.exponent << ")\n"
    "  exponent:\n"
    "    actual          = " << test_case.expected_.exponent         << "\n"
    "    expected        = " << test_case.actual_  .exponent         << "\n"
    "  mantissa:\n"
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