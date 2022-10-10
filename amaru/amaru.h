#pragma once

#ifndef AMARU_FUNCTION
#error "Invalid inclusion of amaru.h."
#endif

#include "common.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static_assert(CHAR_BIT * sizeof(suint_t) == size,
  "Size of suint_t does not match what the generator used.");

static_assert(sizeof(duint_t) >= 2 * sizeof(suint_t),
  "duint_t must be at least twice the size of suint_t.");

static inline
rep_t make_decimal(bool const is_negative, int32_t exponent, suint_t mantissa) {
  rep_t const decimal = { is_negative, exponent, mantissa };
  return decimal;
}

static inline
rep_t remove_trailing_zeros(bool const negative, int32_t exponent,
  suint_t mantissa) {

  suint_t const minv5 = -(((suint_t) -1) / 5);

  mantissa = (mantissa * minv5) / 2;
  ++exponent;

  suint_t product = mantissa * minv5;
  while (product % 2 == 0 && product <= -minv5) {
    ++exponent;
    mantissa = product / 2;
    product  = mantissa * minv5;
  }
  return make_decimal(negative, exponent, mantissa);
}

static inline
suint_t multipliy_and_shift(suint_t const m, suint_t const upper,
  suint_t const lower) {
  duint_t const upper_m = ((duint_t) upper) * m;
  duint_t const lower_m = ((duint_t) lower) * m;
  return (upper_m + (lower_m >> size)) >> (shift - size);
}

static inline
bool is_multiple_of_pow5(suint_t const m, int32_t const f) {
  return f >= 0 && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    m * minverse[f].multiplier <= minverse[f].bound;
}

rep_t AMARU_FUNCTION(bool const is_negative, int32_t const exponent,
  suint_t const mantissa) {

  suint_t  const normal_mantissa_min = AMARU_POW2(suint_t, mantissa_size);
  duint_t  const inv10               = AMARU_POW2(duint_t, size) / 10 + 1;

//  if (exponent == bin_exponent_min && mantissa == 0)
//    return make_decimal(is_negative, 0, 0);

  int32_t  const f     = log10_pow2(exponent);
  uint32_t const extra = is_compact ? log10_pow2_remainder(exponent) : 0;
  int32_t  const i     = is_compact ? f - dec_exponent_min :
    exponent - bin_exponent_min;
  suint_t  const upper = multipliers[i].upper;
  suint_t  const lower = multipliers[i].lower;

  if (mantissa != normal_mantissa_min || exponent == bin_exponent_min) {

    suint_t const m_b  = (2 * mantissa + 1) << extra;
    suint_t const b    = multipliy_and_shift(m_b, upper, lower);

    suint_t const m_a  = (2 * mantissa - 1) << extra;
    suint_t const a    = multipliy_and_shift(m_a, upper, lower);

    suint_t const s    = 10 * ((inv10 * b) >> size);

    if (s < a)
      ;
    else if (s == a) {
      if (mantissa % 2 == 0 && is_multiple_of_pow5(m_a, f))
         return remove_trailing_zeros(is_negative, f, s);
    }
    else if (s == b) {
      if (mantissa % 2 == 0 || !is_multiple_of_pow5(m_b, f))
        return remove_trailing_zeros(is_negative, f, s);
    }
    else // a < s < b
      return remove_trailing_zeros(is_negative, f, s);

    if ((a ^ b) % 2 == 1)
      return make_decimal(is_negative, f, (a + b) / 2 + 1);

    suint_t const m_c = 2 * 2 * mantissa;
    suint_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
    suint_t const c   = c_2 / 2;

    if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f)))
      return make_decimal(is_negative, f, c + 1);

    return make_decimal(is_negative, f, c);
  }

  suint_t const m_b = 2 * normal_mantissa_min + 1;
  suint_t const b   = multipliy_and_shift(m_b << extra, upper, lower);

  suint_t const m_a = 4 * normal_mantissa_min - 1;
  suint_t const a   = multipliy_and_shift(m_a << extra, upper, lower) / 2 +
    !is_multiple_of_pow5(m_a, f);

  if (b >= a) {

    suint_t const s = 10 * ((inv10 * b) >> size);

    if (s >= a)
      return remove_trailing_zeros(is_negative, f, s);

    suint_t const m_c = 2 * 2 * normal_mantissa_min;
    suint_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
    suint_t const c   = c_2 / 2;

    if (c < a || (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f))))
      return make_decimal(is_negative, f, c + 1);

    return make_decimal(is_negative, f, c);
  }

  suint_t const m_c = 2 * 20 * normal_mantissa_min;
  suint_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
  suint_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f)))
    return make_decimal(is_negative, f - 1, c + 1);

  return make_decimal(is_negative, f - 1, c);
}

#ifdef __cplusplus
}
#endif
