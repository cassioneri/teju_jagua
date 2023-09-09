#ifndef AMARU_TEST_TEST_CASE_H_
#define AMARU_TEST_TEST_CASE_H_

#include "amaru/config.h"
#include "test/fp_traits.hpp"

#include <iosfwd>

namespace amaru {
namespace test {

template <typename T>
struct test_case_t;

template <typename T>
std::ostream&
operator <<(std::ostream& os, test_case_t<T> const& test_case);

template <typename T>
struct test_case_t {

  using traits_t = fp_traits_t<T>;
  using fields_t = typename traits_t::fields_t;

  explicit test_case_t(T value, fields_t const& expected,
    fields_t const& actual);

  explicit test_case_t(fields_t const& ieee, fields_t const& expected,
    fields_t const& actual);

  explicit operator bool() const;

  friend std::ostream&
  operator << <T>(std::ostream& os, test_case_t<T> const& test_case);

private:

  T        value_;
  fields_t actual_;
  fields_t expected_;

}; // test_case_t

} // namespace test
} // namespace amaru

#endif // AMARU_TEST_TEST_CASE_H_
