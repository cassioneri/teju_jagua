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
static suint_t  const inv10 = AMARU_POW2(duint_t, ssize) / 10 + 1;

static inline
rep_t remove_trailing_zeros(bool const negative, int32_t exponent,
  suint_t mantissa) {

  duint_t product = ((duint_t) inv10) * mantissa;

  do {
    ++exponent;
    mantissa = (suint_t) (product >> ssize);
    product  = ((duint_t) inv10) * mantissa;
  } while ((suint_t) product < inv10);

  rep_t const decimal = { negative, exponent, mantissa };
  return decimal;
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
  is_multiple_of_pow5(m, upper, lower, shift + e0)

#endif

rep_t AMARU_IMPL(bool const negative, int32_t const exponent,
  suint_t const mantissa) {

  if (exponent == bin_exponent_min && mantissa == 0) {
    rep_t const decimal = { negative, 0, 0 };
    return decimal;
  }

#if defined(AMARU_IDENTIFY_SPECIAL_CASES)
  if (exponent == 0 || exponent == 1) {
    suint_t const m = mantissa << exponent;
    if (m % 10 == 0)
      return remove_trailing_zeros(negative, 0, m);
    rep_t const decimal = { negative, 0, m };
    return decimal;
  }
#endif

  int32_t  const f = log10_pow2(exponent);
  int32_t  const e = exponent - f;

#if defined(AMARU_USE_COMPACT_TBL)
  int32_t  const e0    = log2_pow10(f) - f;
  uint32_t const extra = e - e0;
  int32_t  const i     = f - dec_exponent_min;
#else
  int32_t  const e0    = e;
  uint32_t const extra = 0;
  int32_t  const i     = exponent - bin_exponent_min;
#endif

  // Disables unused variable warning when AMARU_USE_MINVERSE is defined.
  (void) e0;

#if defined(AMARU_UPPER_IS_ZERO)
  duint_t  const upper = 0;
#else
  duint_t  const upper = multipliers[i].upper;
#endif
  duint_t  const lower = multipliers[i].lower;
#if defined(AMARU_SHIFT)
  uint32_t const shift = AMARU_SHIFT;
#else
  uint32_t const shift = multipliers[i].shift;
#endif

  // The below doesn't overflow. (See generator's overflow check #1).
  suint_t const m_b   = 2 * mantissa + 1;
  suint_t const b_hat = multipliy_and_shift(m_b << extra, upper, lower, shift);
  suint_t const b     = b_hat / 2;

  if (mantissa != normal_mantissa_min || exponent == bin_exponent_min) {

    suint_t const r_b = inv10 * b;

    if (r_b >= inv10) {

      suint_t const m_a   = 2 * mantissa - 1;
      suint_t const a_hat = multipliy_and_shift(m_a << extra, upper, lower,
        shift);
      suint_t const a     = a_hat / 2;

      if (a == b) {
        rep_t const decimal = { negative, f, a };
        return decimal;
      }

      suint_t const r_a = inv10 * a;

      if (r_b - 4 <= r_a)
        return remove_trailing_zeros(negative, f, b);

      else if (r_a < inv10 && e > 0 && f <= dec_exponent_critical &&
        a_hat % 2 == 0 && mantissa % 2 == 0 && AMARU_IS_MULTIPLE_OF_POW5(m_a))
        return remove_trailing_zeros(negative, f, a);
    }

    else if (e <= 0 || f > dec_exponent_critical || b_hat % 2 == 1 ||
      mantissa % 2 == 0 || !AMARU_IS_MULTIPLE_OF_POW5(m_b))
      return remove_trailing_zeros(negative, f, b);

    suint_t const m_c   = 2 * mantissa;
    suint_t const c_hat = multipliy_and_shift(m_c << extra, upper, lower,
      shift);
    suint_t const c     = c_hat / 2;

    if (c_hat % 2 == 1 && (c % 2 == 1 || !(-((int32_t) (mantissa_size + 2)) < e
      && e < 0 && m_c % AMARU_POW2(suint_t, -e) == 0))) {
      rep_t const decimal = { negative, f, c + 1 };
      return decimal;
    }

    rep_t const decimal = { negative, f, c };
    return decimal;
  }

  // mantissa = normal_mantissa_min

  // The below doesn't overflow. (See generator's overflow check #2).
  suint_t const m_a      = 4 * normal_mantissa_min - 1;
  suint_t const a_hat    = multipliy_and_shift(m_a << extra, upper, lower,
    shift);
  bool    const is_exact = mantissa_size % 4 == 2 && e > 1 &&
    f <= dec_exponent_critical && a_hat % 4 == 0 &&
    AMARU_IS_MULTIPLE_OF_POW5(m_a);
  suint_t const a        = a_hat / 4 + !is_exact;

  if (b >= a) {

    suint_t const s = 10 * (b / 10);

    if (s >= a)
      return remove_trailing_zeros(negative, f, s);

    suint_t const m_c   = 2 * normal_mantissa_min;
    suint_t const c_hat = multipliy_and_shift(m_c << extra, upper, lower,
      shift);
    suint_t const c     = c_hat / 2;

    if (c < a || (c_hat % 2 == 1 && (c % 2 == 1 ||
      !(-((int32_t) (mantissa_size + 1)) <= e && f <= 0)))) {
      rep_t const decimal = { negative, f, c + 1 };
      return decimal;
    }

    rep_t const decimal = { negative, f, c  };
    return decimal;
  }

  // The below doesn't overflow. (See generator's overflow check #2).
  suint_t const m_c     = 20 * normal_mantissa_min;
  suint_t const c_hat   = multipliy_and_shift(m_c << extra, upper, lower,
    shift);
  suint_t const c       = c_hat / 2;

  if (c_hat % 2 == 1 && (c % 2 == 1 || !(-((int32_t) (mantissa_size + 1)) <= e
    && f <= 1))) {
    rep_t const decimal = { negative, f - 1, c + 1 };
    return decimal;
  }

  rep_t const decimal = { negative, f - 1, c };
  return decimal;
}

#ifdef __cplusplus
}
#endif

#undef AMARU_IS_MULTIPLE_OF_POW5
