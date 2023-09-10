#ifndef AMARU_CPP_COMMON_FIELDS_HPP_
#define AMARU_CPP_COMMON_FIELDS_HPP_

#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>

namespace amaru {

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
