#ifndef AMARU_AMAHU_AMARU_H_
#define AMARU_AMAHU_AMARU_H_

#include "amaru/common.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
amaru_fields_t
make_decimal(int32_t exponent, amaru_limb1_t mantissa) {
  amaru_fields_t const decimal = { exponent, mantissa };
  return decimal;
}

static inline
amaru_limb1_t
rotr(amaru_limb1_t const n, unsigned s) {
  return (n >> s) | (n << (amaru_size - s));
}

static inline
amaru_fields_t
remove_trailing_zeros(int32_t exponent, amaru_limb1_t mantissa) {

  amaru_limb1_t const minv5  = -(((amaru_limb1_t) -1) / 5);
  amaru_limb1_t const minv25 = minv5 * minv5;
  amaru_limb1_t const inv10  = ((amaru_limb1_t) -1) / 10;
  amaru_limb1_t const inv100 = ((amaru_limb1_t) -1) / 100;

  while (true) {

    amaru_limb1_t const n = rotr(minv25 * mantissa, 2);
    if (n > inv100)
      break;

    exponent += 2;
    mantissa = n;
  }

  amaru_limb1_t const n = rotr(minv5 * mantissa, 1);
  if (n <= inv10) {
    ++exponent;
    mantissa = n;
  }

  return make_decimal(exponent, mantissa);
}

static inline
amaru_limb1_t
div10(amaru_limb1_t const m) {
  amaru_limb2_t const inv10 = ((amaru_limb1_t) -1) / 10 + 1;
  return (inv10 * m) >> amaru_size;
}

static inline
amaru_limb1_t
infimum(amaru_limb1_t const m, amaru_limb1_t const h,
  amaru_limb1_t const l) {
  amaru_limb2_t const pu = ((amaru_limb2_t) h) * m;
  amaru_limb2_t const pl = ((amaru_limb2_t) l) * m;
  return (pu + (pl >> amaru_size)) >>
    (amaru_calculation_shift - amaru_size);
}

static inline
bool
is_multiple_of_pow5(amaru_limb1_t const m, int32_t const f) {
  return f >= 0 && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    m * minverse[f].multiplier <= minverse[f].bound;
}

static inline
bool
is_multiple_of_pow2(amaru_limb1_t const m, int32_t const e) {
  return 0 <= e && e <= amaru_mantissa_size && ((m >> e) << e) == m;
}

amaru_fields_t
amaru_function(int32_t const exponent, amaru_limb1_t const mantissa) {

  if (amaru_optimisation_integer &&
    is_multiple_of_pow2(mantissa, -exponent))
    return remove_trailing_zeros(0, mantissa >> -exponent);

  amaru_limb1_t const mantissa_min =
    AMARU_POW2(amaru_limb1_t, amaru_mantissa_size);

  int32_t       const f     = log10_pow2(exponent);
  uint32_t      const extra = amaru_storage_is_compact ?
    log10_pow2_remainder(exponent) : 0;
  int32_t       const i     = (amaru_storage_is_compact ? f : exponent) -
    amaru_storage_index_offset;
  amaru_limb1_t const upper = multipliers[i].upper;
  amaru_limb1_t const lower = multipliers[i].lower;

  if (mantissa != mantissa_min || exponent == amaru_exponent_minimum) {

    amaru_limb1_t const m_b = (2 * mantissa + 1) << extra;
    amaru_limb1_t const b   = infimum(m_b, upper, lower);

    amaru_limb1_t const m_a = (2 * mantissa - 1) << extra;
    amaru_limb1_t const a   = infimum(m_a, upper, lower);

    amaru_limb1_t const q   = div10(b);
    amaru_limb1_t const s   = 10 * q;

    if (s >= a) {

      if (s == b) {
        if (mantissa % 2 == 0 || !is_multiple_of_pow5(m_b, f))
          return remove_trailing_zeros(f + 1, q);
      }

      else if (s > a)
        return remove_trailing_zeros(f + 1, q);

      else if (mantissa % 2 == 0 && is_multiple_of_pow5(m_a, f)) // s == a
        return remove_trailing_zeros(f + 1, q);
    }

    if (amaru_optimisation_mid_point && (a ^ b) % 2 == 1)
      return make_decimal(f, (a + b) / 2 + 1);

    amaru_limb1_t const m_c = 2 * 2 * mantissa;
    amaru_limb1_t const c_2 = infimum(m_c << extra, upper, lower);
    amaru_limb1_t const c   = c_2 / 2;

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_multiple_of_pow5(c_2, -f)))
      return make_decimal(f, c);

    return make_decimal(f, c + 1);
  }

  amaru_limb1_t const m_b = 2 * mantissa_min + 1;
  amaru_limb1_t const b   = infimum(m_b << extra, upper, lower);

  amaru_limb1_t const m_a = 4 * mantissa_min - 1;
  amaru_limb1_t const a   = infimum(m_a << extra, upper, lower) / 2;

  if (b > a) {

    amaru_limb1_t const q = div10(b);
    amaru_limb1_t const s = 10 * q;

    if (s > a || (s == a && is_multiple_of_pow5(m_a, f)))
      return remove_trailing_zeros(f + 1, q);

    amaru_limb1_t const m_c = 2 * 2 * mantissa_min;
    amaru_limb1_t const c_2 = infimum(m_c << extra, upper, lower);
    amaru_limb1_t const c   = c_2 / 2;

    if (c == a && !is_multiple_of_pow5(m_a, f))
      return make_decimal(f, c + 1);

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_multiple_of_pow5(c_2, -f)))
      return make_decimal(f, c);

    return make_decimal(f, c + 1);
  }

  else if (b == a) {
    if (is_multiple_of_pow5(m_a, f)) {
      amaru_limb1_t const q = div10(a);
      return 10 * q == a ? remove_trailing_zeros(f + 1, q) :
        make_decimal(f, a);
    }
  }

  amaru_limb1_t const m_c = 2 * 20 * mantissa_min;
  amaru_limb1_t const c_2 = infimum(m_c << extra, upper, lower);
  amaru_limb1_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f)))
    return make_decimal(f - 1, c + 1);

  return make_decimal(f - 1, c);
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_AMARU_H_
