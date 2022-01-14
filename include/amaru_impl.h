#ifndef AMARU_IMPL
#error "Invalid inclusion of amaru_impl.h."
#endif

#include "common.h"

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static_assert(sizeof(duint_t) >= 2 * sizeof(suint_t),
  "duint_t must be at least twice the size of suint_t.");

static uint32_t const ssize = CHAR_BIT * sizeof(suint_t);
static uint32_t const dsize = CHAR_BIT * sizeof(duint_t);

static int32_t const dec_exponent_critical = AMARU_LOG5_POW2(mantissa_size + 2);
static int32_t const dec_exponent_min      = AMARU_LOG10_POW2(bin_exponent_min);
static suint_t const normal_mantissa_min   = AMARU_POW2(suint_t, mantissa_size);

static inline
rep_t make_decimal(bool const negative, int32_t exponent, suint_t mantissa) {
  rep_t const decimal = { negative, exponent, mantissa };
  return decimal;
}

static inline
rep_t remove_trailing_zeros(bool const negative, int32_t exponent,
  suint_t mantissa) {

  suint_t const inv10 = AMARU_POW2(duint_t, ssize) / 10 + 1;
  duint_t product     = ((duint_t) inv10) * mantissa;

  do {
    ++exponent;
    mantissa = (suint_t) (product >> ssize);
    product  = ((duint_t) inv10) * mantissa;
  } while ((suint_t) product < inv10);

  return make_decimal(negative, exponent, mantissa);
}

static inline
suint_t multipliy_and_shift(suint_t const m, duint_t const upper,
  duint_t const lower, uint32_t const shift) {

  duint_t const upper_m = upper * m;
  duint_t const lower_m = lower * m;

  if (shift >= ssize)
    return (upper_m + (lower_m >> ssize)) >> (shift - ssize);

  return (lower_m >> shift) + (upper_m << (ssize - shift));
}

#if defined(AMARU_USE_MINVERSE)

static inline
bool is_multiple_of_pow5(suint_t const m, int32_t const f) {
  return f == 0 || m * minverse[f - 1].multiplier < minverse[f - 1].bound;
}

#define AMARU_IS_MULTIPLE_OF_POW5(m) is_multiple_of_pow5(m, f)

#else

static inline
suint_t is_multiple_of_pow5(suint_t const m, duint_t const upper,
  duint_t const lower, uint32_t const n_bits) {

  duint_t const multiplier = (upper << ssize) + lower;
  duint_t const upper_m    = upper * m;
  duint_t const lower_m    = lower * m;

  if (n_bits >= dsize) {

    duint_t const upper_limbs = upper_m + (lower_m >> ssize);

    if (AMARU_LSB(upper_limbs, n_bits - ssize) >= AMARU_POW2(duint_t, ssize))
      return false;

    duint_t const lower_limb = (suint_t) lower_m;
    duint_t const product    = (upper_limbs << ssize) + lower_limb;
    return product < multiplier;
  }

  duint_t const product = (upper_m << ssize) + lower_m;
  return AMARU_LSB(product, n_bits) < multiplier;
}

#define AMARU_IS_MULTIPLE_OF_POW5(m) \
  is_multiple_of_pow5(m, upper, lower, shift + e)

#endif

rep_t AMARU_IMPL(bool const negative, int32_t const exponent,
  suint_t const mantissa) {

  if (exponent == bin_exponent_min && mantissa == 0)
    return make_decimal(negative, 0, 0);

#if defined(AMARU_IDENTIFY_SPECIAL_CASES)
  if (exponent == 0 || exponent == 1) {
    suint_t const m = mantissa << exponent;
    if (m % 10 == 0)
      return remove_trailing_zeros(negative, 0, m);
    return make_decimal(negative, 0, m);
  }
#endif

  int32_t  const f = AMARU_LOG10_POW2(exponent);
  int32_t  const e = exponent - f;

#if defined(AMARU_USE_COMPACT_TBL)
  uint32_t const extra = exponent + AMARU_LOG2_POW10(-f);
  int32_t  const i     = f - dec_exponent_min;
#else
  uint32_t const extra = 0;
  int32_t  const i     = exponent - bin_exponent_min;
#endif

#if defined(AMARU_UPPER_IS_ZERO)
  duint_t  const upper = 0;
#else
  duint_t  const upper = multipliers[i].upper;
#endif
  duint_t  const lower = multipliers[i].lower;
#if defined(AMARU_SHIFT)
  uint32_t const shift = AMARU_SHIFT - extra;
#else
  uint32_t const shift = multipliers[i].shift - extra;
#endif

  suint_t const m_b = 2 * mantissa + 1;
  suint_t const b_2 = multipliy_and_shift(m_b, upper, lower, shift);

  if (mantissa != normal_mantissa_min || exponent == bin_exponent_min) {

    suint_t const s   = 10 * (b_2 / 20);
    suint_t const m_a = 2 * mantissa - 1;
    suint_t const a   = multipliy_and_shift(m_a, upper, lower, shift) / 2;

    if (a < s) {
      if (e < 2 || f + 1 > dec_exponent_critical ||
        !is_multiple_of_pow5(m_b, f + 1) || mantissa % 2 == 0)
        return remove_trailing_zeros(negative, f, s);
    }

    else if (s == a && e > 0 && f <= dec_exponent_critical &&
      is_multiple_of_pow5(m_a, f) && mantissa % 2 == 0)
      return remove_trailing_zeros(negative, f, s);

    suint_t const m_c = 2 * mantissa;
    suint_t const c_2 = multipliy_and_shift(m_c, upper, lower, shift);
    suint_t const c   = c_2 / 2;

    if (c_2 % 2 == 1 && (c % 2 == 1 || !(-((int32_t) (mantissa_size + 2)) < e &&
      e < 0 && m_c % AMARU_POW2(suint_t, -e) == 0)))
      return make_decimal(negative, f, c + 1);

    return make_decimal(negative, f, c);
  }

  // mantissa = normal_mantissa_min

  suint_t const b   = b_2 / 2;
  suint_t const m_a = 4 * normal_mantissa_min - 1;
  suint_t const a_2 = multipliy_and_shift(m_a, upper, lower, shift);

  bool const is_exact = mantissa_size % 4 == 2 && e > 1 &&
    f <= dec_exponent_critical && a_2 % 4 == 0 &&
    AMARU_IS_MULTIPLE_OF_POW5(m_a);

  suint_t const a  = a_2 / 4 + !is_exact;

  if (b >= a) {

    suint_t const s = 10 * (b / 10);

    if (s >= a)
      return remove_trailing_zeros(negative, f, s);

    suint_t const m_c = 2 * normal_mantissa_min;
    suint_t const c_2 = multipliy_and_shift(m_c, upper, lower, shift);
    suint_t const c   = c_2 / 2;

    if (c < a || (c_2 % 2 == 1 && (c % 2 == 1 ||
      !(-((int32_t) (mantissa_size + 1)) <= e && f <= 0))))
      return make_decimal(negative, f, c + 1);

    return make_decimal(negative, f, c);
  }

  // The below doesn't overflow. (See generator's overflow check #2).
  suint_t const m_c = 20 * normal_mantissa_min;
  suint_t const c_2 = multipliy_and_shift(m_c, upper, lower, shift);
  suint_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || !(-((int32_t) (mantissa_size + 1)) <= e
    && f <= 1)))
    return make_decimal(negative, f - 1, c + 1);

  return make_decimal(negative, f - 1, c);
}

#ifdef __cplusplus
}
#endif

#undef AMARU_IS_MULTIPLE_OF_POW5
