#ifndef AMARU_AMAHU_AMARU_H_
#define AMARU_AMAHU_AMARU_H_

#if AMARU_SIZE == 32

#define AMARU_MAX_LIMBS AMARU32_MAX_LIMBS

typedef amaru32_limb1_t amaru_limb1_t;
#if AMARU_MAX_LIMBS >= 2
typedef amaru32_limb2_t amaru_limb2_t;
#elif AMARU_MAX_LIMBS >= 4
typedef amaru32_limb4_t amaru_limb4_t;
#endif

typedef amaru32_fields_t amaru_fields_t;

#elif AMARU_SIZE == 64

#define AMARU_MAX_LIMBS AMARU64_MAX_LIMBS

typedef amaru64_limb1_t amaru_limb1_t;
#if AMARU_MAX_LIMBS >= 2
typedef amaru64_limb2_t amaru_limb2_t;
#elif AMARU_MAX_LIMBS >= 4
typedef amaru64_limb4_t amaru_limb4_t;
#endif
typedef amaru64_fields_t amaru_fields_t;

#endif

#include "amaru/common.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
amaru_fields_t make_decimal(bool const is_negative, int32_t exponent,
  amaru_limb1_t mantissa) {
  amaru_fields_t const decimal = { is_negative, exponent, mantissa };
  return decimal;
}

static inline
amaru_fields_t remove_trailing_zeros(bool const negative, int32_t exponent,
  amaru_limb1_t mantissa) {

  amaru_limb1_t const minv5 = -(((amaru_limb1_t) -1) / 5);

  mantissa = (mantissa * minv5) / 2;
  ++exponent;

  amaru_limb1_t product = mantissa * minv5;
  while (product % 2 == 0 && product <= -minv5) {
    ++exponent;
    mantissa = product / 2;
    product  = mantissa * minv5;
  }
  return make_decimal(negative, exponent, mantissa);
}

#if AMARU_MAX_LIMBS >= 2
static inline
amaru_limb1_t multipliy_and_shift(amaru_limb1_t const m,
  amaru_limb1_t const upper, amaru_limb1_t const lower) {
  amaru_limb2_t const upper_m = ((amaru_limb2_t) upper) * m;
  amaru_limb2_t const lower_m = ((amaru_limb2_t) lower) * m;
  return (upper_m + (lower_m >> size)) >> (shift - size);
}
#endif

static inline
bool is_multiple_of_pow5(amaru_limb1_t const m, int32_t const f) {
  return f >= 0 && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    m * minverse[f].multiplier <= minverse[f].bound;
}

amaru_fields_t AMARU_FUNCTION(bool const is_negative, int32_t const exponent,
  amaru_limb1_t const mantissa) {

  amaru_limb1_t const mantissa_min = AMARU_POW2(amaru_limb1_t, mantissa_size);
  amaru_limb2_t const inv10        = ((amaru_limb1_t) -1) / 10 + 1;

  int32_t  const f     = log10_pow2(exponent);
  uint32_t const extra = is_compact ? log10_pow2_remainder(exponent) : 0;
  int32_t  const i     = (is_compact ? f : exponent) - index_offset;
  amaru_limb1_t  const upper = multipliers[i].upper;
  amaru_limb1_t  const lower = multipliers[i].lower;

  if (mantissa != mantissa_min || exponent == exponent_min) {

    amaru_limb1_t const m_b = (2 * mantissa + 1) << extra;
    amaru_limb1_t const b   = multipliy_and_shift(m_b, upper, lower);

    amaru_limb1_t const m_a = (2 * mantissa - 1) << extra;
    amaru_limb1_t const a   = multipliy_and_shift(m_a, upper, lower);

    amaru_limb1_t const s   = 10 * ((inv10 * b) >> size);

    if (s < a)
      ;

    else if (s == b) {
      if (mantissa % 2 == 0 || !is_multiple_of_pow5(m_b, f))
        return remove_trailing_zeros(is_negative, f, s);
    }

    else if (s > a)
      return remove_trailing_zeros(is_negative, f, s);

    else /* s == a */ if (mantissa % 2 == 0 && is_multiple_of_pow5(m_a, f))
       return remove_trailing_zeros(is_negative, f, s);

    if ((a ^ b) % 2 == 1)
      return make_decimal(is_negative, f, (a + b) / 2 + 1);

    amaru_limb1_t const m_c = 2 * 2 * mantissa;
    amaru_limb1_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
    amaru_limb1_t const c   = c_2 / 2;

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_multiple_of_pow5(c_2, -f)))
      return make_decimal(is_negative, f, c);

    return make_decimal(is_negative, f, c + 1);
  }

  amaru_limb1_t const m_b = 2 * mantissa_min + 1;
  amaru_limb1_t const b   = multipliy_and_shift(m_b << extra, upper, lower);

  amaru_limb1_t const m_a = 4 * mantissa_min - 1;
  amaru_limb1_t const a   = multipliy_and_shift(m_a << extra, upper, lower) / 2
    + !is_multiple_of_pow5(m_a, f);

  if (b >= a) {

    amaru_limb1_t const s = 10 * ((inv10 * b) >> size);

    if (s >= a)
      return remove_trailing_zeros(is_negative, f, s);

    amaru_limb1_t const m_c = 2 * 2 * mantissa_min;
    amaru_limb1_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
    amaru_limb1_t const c   = c_2 / 2;

    if (c < a || (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f))))
      return make_decimal(is_negative, f, c + 1);

    return make_decimal(is_negative, f, c);
  }

  amaru_limb1_t const m_c = 2 * 20 * mantissa_min;
  amaru_limb1_t const c_2 = multipliy_and_shift(m_c << extra, upper, lower);
  amaru_limb1_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f)))
    return make_decimal(is_negative, f - 1, c + 1);

  return make_decimal(is_negative, f - 1, c);
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_AMARU_H_
