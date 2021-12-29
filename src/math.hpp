#pragma once

#include <boost/multiprecision/cpp_int.hpp>

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

/**
 * \brief Returns 2^n.
 */
integer_t pow2(uint32_t n) {
  return integer_t{1} << n;
}

/**
 * \brief Returns 5^n.
 */
integer_t pow5(uint32_t n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

/**
 * \brief Returns the integer part of log_5(2^n).
 *
 * \pre -227267 <= n && n < 227268.
 */
int32_t log5_pow2(int32_t n) {
  uint64_t const log5_2_times_2_to_32 = 1849741732;
  return log5_2_times_2_to_32 * n >> 32;
}
