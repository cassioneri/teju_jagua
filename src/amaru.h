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

enum {
  ssize = CHAR_BIT * sizeof(suint_t),
  dsize = CHAR_BIT * sizeof(duint_t),
};

static inline
rep_t remove_trailing_zeros(rep_t decimal) {

  suint_t const m = (~((suint_t)0)) / 10 + 1;
  duint_t       p = ((duint_t) m) * decimal.mantissa;

  do {
    ++decimal.exponent;
    decimal.mantissa = (suint_t) (p >> ssize);
    p                = ((duint_t) m) * decimal.mantissa;
  } while ((suint_t) p < m);

  return decimal;
}

static inline
duint_t pack(duint_t upper, duint_t lower) {
  return (upper << ssize) + lower;
}

static inline
suint_t scale(duint_t const upper_limbs, suint_t const lower_limb,
  uint32_t const shift) {
  if (shift >= ssize)
    return upper_limbs >> (shift - ssize);
  return (lower_limb >> shift) | (upper_limbs << (ssize - shift));
}

static inline
suint_t is_multiple_of_pow5(duint_t const multiplier, uint32_t const n_bits,
  duint_t const upper_prod, duint_t const lower_prod,
  duint_t const upper_limbs) {

  if (n_bits >= dsize) {

    if (AMARU_LOWER_BITS(upper_limbs, n_bits - ssize) >> ssize > 0)
      return false;

    duint_t const lower_limb = (suint_t) lower_prod;
    duint_t const prod       = pack(upper_limbs, lower_limb);
    return prod < multiplier;
  }

  duint_t const prod = pack(upper_prod, lower_prod);

  return AMARU_LOWER_BITS(prod, n_bits) < multiplier;
}

rep_t
TO_AMARU_DEC(bool const negative, int32_t const exponent,
  suint_t const mantissa) {

  rep_t decimal;
  decimal.negative = negative;

  if (exponent == exponent_min && mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
    return decimal;
  }

  decimal.exponent = log10_pow2(exponent);

  int32_t  const e             = exponent - decimal.exponent;

  uint32_t const index         = exponent - exponent_min;
  duint_t  const upper         = scalers[index].upper;
  duint_t  const lower         = scalers[index].lower;
  uint32_t const shift         = scalers[index].shift;

  suint_t  const m_b           = 2 * mantissa + 1;
  duint_t  const upper_prod_b  = upper * m_b;
  duint_t  const lower_prod_b  = lower * m_b;
  duint_t  const upper_limbs_b = upper_prod_b + (lower_prod_b >> ssize);

  suint_t  const b_hat         = scale(upper_limbs_b, lower_prod_b, shift);
  suint_t  const b             = b_hat / 2;

  bool shorten;

  if (mantissa != mantissa_min || exponent == exponent_min) {

    suint_t const s = 10 * (b / 10);

    if (s == b) {
      bool const is_exact = exponent > 0 &&
        decimal.exponent <= exponent_critical && b_hat % 2 == 0 &&
        is_multiple_of_pow5(pack(upper, lower), shift + e, upper_prod_b,
          lower_prod_b, upper_limbs_b);
      shorten = !is_exact || mantissa % 2 == 0;
    }

    else {
      // m_a = 2 * mantissa - 1 = m_b - 2.
      duint_t const upper_prod_a  = upper_prod_b - 2 * upper;
      duint_t const lower_prod_a  = lower_prod_b - 2 * lower;
      duint_t const upper_limbs_a = upper_prod_a + (lower_prod_a >> ssize);
      suint_t const a_hat         = scale(upper_limbs_a, lower_prod_a, shift);

      bool const is_exact = exponent > 0 &&
        decimal.exponent <= exponent_critical && a_hat % 2 == 0 &&
        is_multiple_of_pow5(pack(upper, lower), shift + e, upper_prod_a,
          lower_prod_a, upper_limbs_a);
      suint_t const a = a_hat / 2 + !is_exact;
      shorten = s > a || (s == a && (!is_exact || mantissa % 2 == 0));
    }

    if (shorten) {
      decimal.mantissa = s;
      return remove_trailing_zeros(decimal);
    }

    else {
      // m_c = 2 * mantissa = m_b - 1;
      suint_t const m_c           = m_b - 1;
      duint_t const upper_prod_c  = upper_prod_b - upper;
      duint_t const lower_prod_c  = lower_prod_b - lower;
      duint_t const upper_limbs_c = upper_prod_c + (lower_prod_c >> ssize);
      suint_t const c_hat         = scale(upper_limbs_c, lower_prod_c, shift);
      decimal.mantissa            = c_hat / 2;

      if (c_hat % 2 == 1)
        decimal.mantissa += decimal.mantissa % 2 == 1 ||
          !(0 > e && ((uint32_t) -e) < mantissa_size + 2 &&
          m_c % AMARU_POW2(suint_t, -e) == 0);
    }
  }
  else {
    // m_a = 4 * mantissa - 1
    suint_t const m_a           = 4 * mantissa - 1;
    duint_t const upper_prod_a  = upper * m_a;
    duint_t const lower_prod_a  = lower * m_a;
    duint_t const upper_limbs_a = upper_prod_a + (lower_prod_a >> ssize);

    suint_t const a_hat         = scale(upper_limbs_a, lower_prod_a, shift);
    bool    const is_exact      = exponent > 1 &&
      decimal.exponent <= exponent_critical && a_hat % 4 == 0 &&
      is_multiple_of_pow5(pack(upper, lower), shift + e, upper_prod_a, lower_prod_a,
        upper_limbs_a);
    suint_t const a             = a_hat / 4 + !is_exact;

    if (b >= a) {
      suint_t const s = 10 * (b / 10);
      if (s >= a) {
        decimal.mantissa = s;
        return remove_trailing_zeros(decimal);
      }

      else {
        // m_c = 2 * mantissa = m_b - 1
        duint_t const upper_prod_c  = upper_prod_b - upper;
        duint_t const lower_prod_c  = lower_prod_b - lower;
        duint_t const upper_limbs_c = upper_prod_c + (lower_prod_c >> ssize);
        suint_t const c_hat         = scale(upper_limbs_c, lower_prod_c, shift);
        decimal.mantissa            = c_hat / 2;

        if (decimal.mantissa < a)
          ++decimal.mantissa;
        else if (c_hat % 2 == 1)
          decimal.mantissa += decimal.mantissa % 2 == 1 ||
            !(((uint32_t) -e) <= mantissa_size + 1 && decimal.exponent <= 0);
      }
    }
    else {
      --decimal.exponent;
      suint_t const m           = 20 * mantissa;
      duint_t const upper_prod  = upper * m;
      duint_t const lower_prod  = lower * m;
      duint_t const upper_limbs = upper_prod + (lower_prod >> ssize);

      static_assert(CHAR_BIT * sizeof(duint_t) >= mantissa_size + 4,
        "duint is not large enough for calculations to not overflow.");

      suint_t const c_hat = scale(upper_limbs, lower_prod, shift);

      decimal.mantissa = c_hat / 2;
      if (c_hat % 2 == 1)
        decimal.mantissa += decimal.mantissa % 2 == 1 ||
          !(((uint32_t) -e) <= mantissa_size + 1 && decimal.exponent <= 0);
    }
  }
  return decimal;
}

#ifdef __cplusplus
}
#endif
