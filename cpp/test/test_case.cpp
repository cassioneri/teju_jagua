// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"

#include <type_traits>

namespace teju {
namespace test {

namespace {

  template <typename T>
  T
  teju_binary_to_value(cpp_fields_t<T> teju_binary) {

    using traits_t           = teju::traits_t<T>;
    using u1_t               = typename traits_t::u1_t;

    auto const teju_binary_c = teju_binary.c;

    auto const exponent_size = traits_t::exponent_size;
    auto const mantissa_size = traits_t::mantissa_size;

    auto const exponent_min  =
      teju_min_binary_exponent_from_ieee754(exponent_size, mantissa_size);

    auto const exponent_max  =
      teju_max_binary_exponent_from_ieee754(exponent_size, mantissa_size);

    require(teju_binary_c.exponent >= exponent_min,
      "Exponent provided to Teju Jagua binary representation is too low.");

    require(teju_binary_c.exponent <= exponent_max,
      "Exponent provided to Teju Jagua binary representation is too high.");

    auto exponent_ieee =
      static_cast<u1_t>(teju_binary_c.exponent - exponent_min);

    auto const mantissa_bound = teju_pow2(u1_t, mantissa_size);
    auto const subnormal      = exponent_ieee == 0;

    u1_t mantissa_ieee = 0;
    if (subnormal) {
      require(teju_binary_c.mantissa < mantissa_bound,
        "Mantissa provided to Teju Jagua binary representation is too high."
        "(Note: subnormal case.)");
      mantissa_ieee = teju_binary_c.mantissa;
    }
    else {
      require(teju_binary_c.mantissa >= mantissa_bound,
        "Mantissa provided to Teju Jagua binary representation is too low.");
      require(teju_binary_c.mantissa < 2 * mantissa_bound,
        "Mantissa provided to Teju Jagua binary representation is too high.");
      ++exponent_ieee;
      mantissa_ieee = teju_binary_c.mantissa - mantissa_bound;
    }

    auto const bits = (exponent_ieee << mantissa_size) | mantissa_ieee;

    T value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
  }

} // namespace <anonymous>

template <typename T>
test_case_t<T>::test_case_t(T value, fields_t const& expected) :
  value_   {value   },
  expected_{expected} {
}

template <typename T>
test_case_t<T>::test_case_t(fields_t const& teju_binary,
  fields_t const& expected) :
  test_case_t{teju_binary_to_value<T>(teju_binary), expected} {
}

template <typename T>
T const& test_case_t<T>::value() const {
  return value_;
}

template <typename T>
typename test_case_t<T>::fields_t const& test_case_t<T>::expected() const {
  return expected_;
}

template
struct test_case_t<float>;

template
struct test_case_t<double>;

#if defined(TEJU_HAS_FLOAT128)
template
struct test_case_t<float128_t>;
#endif

} // namespace test
} // namespace teju
