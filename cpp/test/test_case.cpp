#include "cpp/common/exception.hpp"
#include "cpp/common/traits.hpp"
#include "cpp/test/test_case.hpp"

#include <type_traits>

namespace amaru {
namespace test {

namespace {

  template <typename T>
  T
  amaru_binary_to_value(cpp_fields_t<T> amaru_binary) {

    using traits_t            = amaru::traits_t<T>;
    using u1_t                = typename traits_t::u1_t;

    auto const amaru_binary_c = amaru_binary.c;

    auto const exponent_size  = traits_t::exponent_size;
    auto const mantissa_size  = traits_t::mantissa_size;

    auto const exponent_min   =
      amaru_min_binary_exponent_from_ieee754(exponent_size, mantissa_size);

    auto const exponent_max   =
      amaru_max_binary_exponent_from_ieee754(exponent_size, mantissa_size);

    require(amaru_binary_c.exponent >= exponent_min,
      "Exponent provided to Amaru binary representation is too low.");

    require(amaru_binary_c.exponent <= exponent_max,
      "Exponent provided to Amaru binary representation is too high.");

    auto exponent_ieee =
      static_cast<u1_t>(amaru_binary_c.exponent - exponent_min);

    auto const mantissa_bound = amaru_pow2(u1_t, mantissa_size);
    auto const subnormal      = exponent_ieee == 0;

    u1_t mantissa_ieee = 0;
    if (subnormal) {
      require(amaru_binary_c.mantissa < mantissa_bound,
        "Mantissa provided to Amaru binary representation is too high."
        "(Note: subnormal case.)");
      mantissa_ieee = amaru_binary_c.mantissa;
    }
    else {
      require(amaru_binary_c.mantissa >= mantissa_bound,
        "Mantissa provided to Amaru binary representation is too low.");
      require(amaru_binary_c.mantissa < 2 * mantissa_bound,
        "Mantissa provided to Amaru binary representation is too high.");
      ++exponent_ieee;
      mantissa_ieee = amaru_binary_c.mantissa - mantissa_bound;
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
test_case_t<T>::test_case_t(fields_t const& amaru_binary,
  fields_t const& expected) :
  test_case_t{amaru_binary_to_value<T>(amaru_binary), expected} {
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

template
struct test_case_t<float128_t>;

} // namespace test
} // namespace amaru
