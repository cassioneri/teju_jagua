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

static_assert(CHAR_BIT * sizeof(suint_t) == ssize,
  "Size of suint_t does not match what the generator used.");

static_assert(sizeof(duint_t) >= 2 * sizeof(suint_t),
  "duint_t must be at least twice the size of suint_t.");

static duint_t const inv10 = AMARU_POW2(duint_t, ssize) / 10 + 1;

static inline
rep_t make_decimal(bool const negative, int32_t exponent, suint_t mantissa) {
  rep_t const decimal = { negative, exponent, mantissa };
  return decimal;
}

static inline
rep_t remove_trailing_zeros(bool const negative, int32_t exponent,
  suint_t mantissa) {

//  suint_t const minv5 = -(((suint_t) -1) / 5);
//
//  mantissa = (mantissa * minv5) / 2;
//  ++exponent;
//
//  duint_t product = inv10 * mantissa;
//  while ((suint_t) product < inv10) {
//    ++exponent;
//    mantissa = (suint_t) (product >> ssize);
//    product  = inv10 * mantissa;
//  }

  duint_t product = inv10 * mantissa;

  do {
    ++exponent;
    mantissa = (suint_t) (product >> ssize);
    product  = inv10 * mantissa;
  } while ((suint_t) product < inv10);

  return make_decimal(negative, exponent, mantissa);
}

static inline
suint_t multipliy_and_shift(suint_t const m, suint_t const upper,
  suint_t const lower, uint32_t const shift) {
  duint_t const upper_m = ((duint_t) upper) * m;
  duint_t const lower_m = ((duint_t) lower) * m;
  return (upper_m + (lower_m >> ssize)) >> shift;
}

static inline
bool is_multiple_of_pow5(suint_t const m, int32_t const f) {
  return f >= 0 && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    m * minverse[f].multiplier <= minverse[f].bound;
}

rep_t AMARU_FUNCTION(bool const negative, int32_t const exponent,
  suint_t const mantissa) {

  static suint_t const normal_mantissa_min = AMARU_POW2(suint_t, mantissa_size);

//  if (exponent == bin_exponent_min && mantissa == 0)
//    return make_decimal(negative, 0, 0);

  int32_t  const f = log10_pow2(exponent);

#if defined(AMARU_USE_COMPACT_TBL)
  uint32_t const extra = log10_pow2_remainder(exponent);
  int32_t  const i     = f - dec_exponent_min;
#else
  uint32_t const extra = 0;
  int32_t  const i     = exponent - bin_exponent_min;
#endif

  int32_t  const e = exponent - f;

  suint_t  const upper     = multipliers[i].upper;
  suint_t  const lower     = multipliers[i].lower;
  uint32_t const adj_shift = shift - extra;

  if (mantissa != normal_mantissa_min || exponent == bin_exponent_min) {

    suint_t const m       = 2 * mantissa - 1;

#if defined(AMARU_USE_COMPACT_TBL)
    duint_t const upper_m = ((duint_t) upper) * m;
    duint_t const lower_m = ((duint_t) lower) * m;
    duint_t const prod_a  = upper_m + (lower_m >> ssize);
    suint_t const a       = prod_a >> adj_shift;

    duint_t const prod_b  = prod_a + (2 * upper + 2);
    suint_t const b       = prod_b >> adj_shift;
#else
    suint_t const a       = multipliy_and_shift(m    , upper, lower, adj_shift);
    suint_t const b       = multipliy_and_shift(m + 2, upper, lower, adj_shift);
#endif

    suint_t const s       = 10 * ((inv10 * b) >> ssize);

    if (s < a)
      ;
    else if (s == a) {
      if (mantissa % 2 == 0 && is_multiple_of_pow5(m, f))
         return remove_trailing_zeros(negative, f, s);
    }
    else if (s == b) {
      if (mantissa % 2 == 0 || !is_multiple_of_pow5(m + 2, f))
        return remove_trailing_zeros(negative, f, s);
    }
    else
      return remove_trailing_zeros(negative, f, s);

    if ((a ^ b) % 2 == 1)
      return make_decimal(negative, f, (a + b) / 2 + 1);

#if defined(AMARU_USE_COMPACT_TBL)
    duint_t const prod_c = prod_a + (upper + 1);
    suint_t const c_2    = prod_c >> (adj_shift - 1);
#else
    suint_t const c_2    = multipliy_and_shift(m + 1, upper, lower, adj_shift - 1);
#endif

    suint_t const c      = c_2 / 2;

    if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2 | 1, -f)))
      return make_decimal(negative, f, c + 1);

    return make_decimal(negative, f, c);
  }

  // mantissa = normal_mantissa_min

  suint_t const m_b = 2 * normal_mantissa_min + 1;
  suint_t const b   = multipliy_and_shift(m_b, upper, lower, adj_shift);

  suint_t const m_a = 4 * normal_mantissa_min - 1;
  suint_t const a_2 = multipliy_and_shift(m_a, upper, lower, adj_shift - 1);

  bool const is_exact = mantissa_size % 4 == 2 && a_2 % 4 == 0 &&
    is_multiple_of_pow5(m_a, f);

  suint_t const a  = a_2 / 4 + !is_exact;

  if (b >= a) {

    suint_t const s = 10 * ((inv10 * b) >> ssize);

    if (s >= a)
      return remove_trailing_zeros(negative, f, s);

    suint_t const m_c = 2 * normal_mantissa_min;
    suint_t const c_2 = multipliy_and_shift(m_c, upper, lower, adj_shift - 1);
    suint_t const c   = c_2 / 2;

    if (c < a || (c_2 % 2 == 1 && (c % 2 == 1 || e > 0 ||
      e < -((int32_t) (mantissa_size + 1)))))
      return make_decimal(negative, f, c + 1);

    return make_decimal(negative, f, c);
  }

  suint_t const m_c = 20 * normal_mantissa_min;
  suint_t const c_2 = multipliy_and_shift(m_c, upper, lower, adj_shift - 1);
  suint_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || (e < -((int32_t) (mantissa_size + 1))
    || f > 1)))
    return make_decimal(negative, f - 1, c + 1);

  return make_decimal(negative, f - 1, c);
}

#ifdef __cplusplus
}
#endif
