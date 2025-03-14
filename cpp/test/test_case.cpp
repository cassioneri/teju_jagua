// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"
#include "teju/common.h"

#include <type_traits>

namespace teju {
namespace test {

template <typename T>
test_case_t<T>::test_case_t(T value, decimal_t<T> const& expected) :
  value_   {value   },
  expected_{expected} {
}

template <typename T>
T const& test_case_t<T>::value() const {
  return value_;
}

template <typename T>
decimal_t<T> const& test_case_t<T>::expected() const {
  return expected_;
}

#if defined(teju_has_float16)
template
struct test_case_t<float16_t>;
#endif

template
struct test_case_t<float>;

template
struct test_case_t<double>;

#if defined(teju_has_float128)
template
struct test_case_t<float128_t>;
#endif

} // namespace test
} // namespace teju
