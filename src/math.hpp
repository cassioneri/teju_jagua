#pragma once

#include <boost/multiprecision/cpp_int.hpp>

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

/**
 * \brief Returns 2^e.
 */
integer_t pow2(uint32_t e) {
  return integer_t{1} << e;
}

/**
 * \brief Returns 5^e.
 */
integer_t pow5(uint32_t e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

/**
 * \brief Returns the integer part of log_5(2^n).
 *
 * \pre n in [-227267, 227268[.
 */
static inline
int32_t log5_pow2(int32_t n) {
  uint64_t const log5_2 = 1849741732;
  return log5_2 * n >> 32;
}
