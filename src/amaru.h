#ifndef TO_AMARU_DEC
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

static_assert(sizeof(duint_t) >= 2 * sizeof(suint_t),
  "duint_t must be at least twice the size of suint_t.");

enum {
  ssize = CHAR_BIT * sizeof(suint_t),
  dsize = CHAR_BIT * sizeof(duint_t),
};

static inline
rep_t remove_trailing_zeros(bool const negative, int32_t exponent,
  suint_t mantissa) {

  suint_t const m = (~((suint_t)0)) / 10 + 1;
  duint_t       p = ((duint_t) m) * mantissa;

  do {
    ++exponent;
    mantissa = (suint_t) (p >> ssize);
    p        = ((duint_t) m) * mantissa;
  } while ((suint_t) p < m);

  rep_t const decimal = { negative, exponent, mantissa };
  return decimal;
}

static inline
duint_t pack(duint_t upper_m, duint_t lower_m) {
  return (upper_m << ssize) + lower_m;
}

static inline
suint_t scale(duint_t const upper_m, duint_t const lower_m,
  uint32_t const shift) {

  if (shift >= ssize)
    return (upper_m + (lower_m >> ssize)) >> (shift - ssize);

  return (lower_m >> shift) + (upper_m << (ssize - shift));
}

static inline
suint_t is_multiple_of_pow5(duint_t const upper_m, duint_t const lower_m,
  duint_t const multiplier, uint32_t const n_bits) {

  if (n_bits >= dsize) {

    duint_t const upper_limbs = upper_m + (lower_m >> ssize);

    if (AMARU_LSB(upper_limbs, n_bits - ssize) >= AMARU_POW2(duint_t, ssize))
      return false;

    duint_t const lower_limb = (suint_t) lower_m;
    duint_t const prod       = pack(upper_limbs, lower_limb);
    return prod < multiplier;
  }

  duint_t const prod = pack(upper_m, lower_m);
  return AMARU_LSB(prod, n_bits) < multiplier;
}

rep_t
TO_AMARU_DEC(bool const negative, int32_t const exponent,
  suint_t const mantissa) {

  if (exponent == exponent_min && mantissa == 0) {
    rep_t decimal = { negative, 0, 0 };
    return decimal;
  }

  int32_t const f = log10_pow2(exponent);
  int32_t const e = exponent - f;
  int32_t const i = exponent - exponent_min;

#ifdef AMARU_UPPER_IS_ZERO
  duint_t  const upper = 0;
#else
  duint_t  const upper = scalers[i].upper;
#endif
  duint_t  const lower = scalers[i].lower;
#ifdef AMARU_SHIFT
  uint32_t const shift = AMARU_SHIFT;
#else
  uint32_t const shift = scalers[i].shift;
#endif

  // The below doesn't overflow. (See generator's overflow check #1).
  suint_t const m_b            = 2 * mantissa + 1;
  duint_t const upper_m_b      = upper * m_b;
  duint_t const lower_m_b      = lower * m_b;
  suint_t const b_hat          = scale(upper_m_b, lower_m_b, shift);
  suint_t const b              = b_hat / 2;
  bool    const might_be_exact = e > 0 && f <= exponent_critical;

  if (mantissa != normal_mantissa_min || exponent == exponent_min) {

    suint_t const s = 10 * (b / 10);

    if (s == b) {
      bool const is_exact = might_be_exact && b_hat % 2 == 0 &&
        is_multiple_of_pow5(upper_m_b, lower_m_b, pack(upper, lower), shift + e);
      if (!is_exact || mantissa % 2 == 0)
        return remove_trailing_zeros(negative, f, s);
    }

    else {
      // m_a = 2 * mantissa - 1 = m_b - 2.
      duint_t const upper_m_a = upper_m_b - 2 * upper;
      duint_t const lower_m_a = lower_m_b - 2 * lower;
      suint_t const a_hat     = scale(upper_m_a, lower_m_a, shift);
      bool    const is_exact  = might_be_exact && a_hat % 2 == 0 &&
        is_multiple_of_pow5(upper_m_a, lower_m_a, pack(upper, lower), shift + e);
      suint_t const a = a_hat / 2 + !is_exact;
      if (s > a || (s == a && (!is_exact || mantissa % 2 == 0)))
        return remove_trailing_zeros(negative, f, s);
    }

    // m_c = 2 * mantissa = m_b - 1;
    suint_t const m_c       = m_b - 1;
    duint_t const upper_m_c = upper_m_b - upper;
    duint_t const lower_m_c = lower_m_b - lower;
    suint_t const c_hat     = scale(upper_m_c, lower_m_c, shift);
    rep_t         decimal   = { negative, f, c_hat / 2 };

    decimal.mantissa += c_hat % 2 == 1 && (decimal.mantissa % 2 == 1 ||
      !(0 > e && ((uint32_t) -e) < mantissa_size + 2 &&
      m_c % AMARU_POW2(suint_t, -e) == 0));

    return decimal;
  }

  // mantissa = mantissa_min

  // The below doesn't overflow. (See generator's overflow check #2).
  suint_t const m_a       = 4 * normal_mantissa_min - 1;
  duint_t const upper_m_a = upper * m_a;
  duint_t const lower_m_a = lower * m_a;
  suint_t const a_hat     = scale(upper_m_a, lower_m_a, shift);
  bool    const is_exact  = might_be_exact && a_hat % 4 == 0 &&
    is_multiple_of_pow5(upper_m_a, lower_m_a, pack(upper, lower), shift + e);
  suint_t const a         = a_hat / 4 + !is_exact;

  if (b >= a) {

    suint_t const s = 10 * (b / 10);

    if (s >= a)
      return remove_trailing_zeros(negative, f, s);

    // m_c = 2 * mantissa = m_b - 1
    duint_t const upper_m_c = upper_m_b - upper;
    duint_t const lower_m_c = lower_m_b - lower;
    suint_t const c_hat     = scale(upper_m_c, lower_m_c, shift);
    rep_t         decimal   = { negative, f, c_hat / 2 };

    if (decimal.mantissa < a)
      ++decimal.mantissa;

    else if (c_hat % 2 == 1)
      decimal.mantissa += decimal.mantissa % 2 == 1 ||
        !(((uint32_t) -e) <= mantissa_size + 1 && decimal.exponent <= 0);

    return decimal;
  }

  // The below doesn't overflow. (See generator's overflow check #2).
  suint_t const m_c       = 20 * normal_mantissa_min;
  duint_t const upper_m_c = upper * m_c;
  duint_t const lower_m_c = lower * m_c;
  suint_t const c_hat     = scale(upper_m_c, lower_m_c, shift);
  rep_t         decimal   = { negative, f - 1, c_hat / 2 };

  if (c_hat % 2 == 1)
    decimal.mantissa += decimal.mantissa % 2 == 1 ||
      !(((uint32_t) -e) <= mantissa_size + 1 && decimal.exponent <= 0);

  return decimal;
}

#ifdef __cplusplus
}
#endif
