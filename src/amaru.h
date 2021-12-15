#ifndef AMARU
#error "Invalid inclusion of amaru.h."
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#define AMARU_DO_RYU   1
#define AMARU_DO_AMARU 1

#ifdef __cplusplus
extern "C" {
#endif

static inline
rep_t remove_trailing_zeros(rep_t decimal) {

  suint_t const m = (~((suint_t)0)) / 10 + 1;

  duint_t p = ((duint_t) m) * decimal.mantissa;
  suint_t r = (suint_t) p;

  while (r < m) {
    ++decimal.exponent;
    decimal.mantissa = (suint_t) (p >> word_size);
    p                = ((duint_t) m) * decimal.mantissa;
    r                = (suint_t) p;
  }
  return decimal;
}

static inline
duint_t add(duint_t upper, duint_t lower) {
  return (((duint_t) upper) << word_size) + lower;
}

static inline
suint_t scale(suint_t const upper, suint_t const lower,
  uint32_t const n_bits, suint_t const m) {

  duint_t const upper_prod  = ((duint_t) upper) * m;
  duint_t const lower_prod  = ((duint_t) lower) * m;

  duint_t const upper_limbs = upper_prod + (lower_prod >> word_size);

  if (n_bits >= word_size)
    return upper_limbs >> (n_bits - word_size);

  suint_t const lower_limb  = (suint_t) lower_prod;
  return (lower_limb >> n_bits) | (upper_limbs << (word_size - n_bits));
}

static inline
suint_t is_multiple_of_pow5(suint_t const m, suint_t const upper,
  suint_t const lower, uint32_t const n_bits) {

  duint_t const lower_prod = ((duint_t) lower) * m;
  duint_t const upper_prod = ((duint_t) upper) * m;

  if (n_bits >= 2*word_size) {

    duint_t const upper_limbs = upper_prod + (lower_prod >> word_size);

    if (AMARU_LOWER_BITS(upper_limbs, n_bits - word_size) >> word_size > 0)
      return false;

    duint_t const lower_limb = (suint_t) lower_prod;
    duint_t const prod       = add(upper_limbs, lower_limb);
    duint_t const multiplier = add(upper, lower);
    return prod < multiplier;
  }

  duint_t const prod       = add(upper_prod, lower_prod);
  duint_t const multiplier = add(upper, lower);

  return AMARU_LOWER_BITS(prod, n_bits) < multiplier;
}

rep_t
AMARU(rep_t const binary) {

  rep_t decimal;
  decimal.negative = binary.negative;

  if (binary.exponent == exponent_min && binary.mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
    return decimal;
  }

  decimal.exponent = amaru_log10_pow2(binary.exponent);

  uint32_t const index     = binary.exponent - exponent_min;
  suint_t  const upper     = scalers[index].upper;
  suint_t  const lower     = scalers[index].lower;
  uint32_t const n_bits    = scalers[index].shift;
  suint_t  const mantissa2 = 2 * binary.mantissa;
  int32_t  const e         = binary.exponent - decimal.exponent;

  suint_t        m         = mantissa2 + 1; // = 2 * binary.mantissa + 1
  suint_t  const b_hat     = scale(upper, lower, n_bits, m);
  suint_t  const b         = b_hat / 2;
  bool shorten;

  if (binary.mantissa != AMARU_POW2(suint_t, mantissa_size) ||
    binary.exponent == exponent_min) {

    suint_t const s = 10 * (b / 10);

    if (s == b) {
      bool const is_exact = binary.exponent > 0 &&
        decimal.exponent <= large_exponent && b_hat % 2 == 0 &&
        is_multiple_of_pow5(m, upper, lower, n_bits + e);
      shorten = !is_exact || binary.mantissa % 2 == 0;
    }

    else {
      m = m - 2; // = 2 * binary.mantissa - 1
      suint_t const a_hat = scale(upper, lower, n_bits, m);
      bool const is_exact = binary.exponent > 0 &&
        decimal.exponent <= large_exponent && a_hat % 2 == 0 &&
        is_multiple_of_pow5(m, upper, lower, n_bits + e);
      suint_t const a = a_hat / 2 + !is_exact;
      shorten = s > a || (s == a && (!is_exact || binary.mantissa % 2 == 0));
    }

    if (shorten) {
      decimal.mantissa = s;
      return remove_trailing_zeros(decimal);
    }

    else {
      m = mantissa2; // = 2 * binary.mantissa
      suint_t const c_hat = scale(upper, lower, n_bits, m);
      decimal.mantissa = c_hat / 2;
      if (c_hat % 2 == 1)
        decimal.mantissa += decimal.mantissa % 2 == 1 ||
          !(0 > e && e > -mantissa_size - 2 && m % AMARU_POW2(suint_t, -e) == 0);
    }
  }
  else {
    m = 2 * m - 3; // = 4 * binary.mantissa - 1
    suint_t const a_hat    = scale(upper, lower, n_bits, m);
    bool    const is_exact = binary.exponent > 1 &&
      decimal.exponent <= large_exponent && a_hat % 4 == 0 &&
      is_multiple_of_pow5(m, upper, lower, n_bits + e);
    suint_t const a        = a_hat / 4 + !is_exact;
    if (b >= a) {
      suint_t const s = 10 * (b / 10);
      if (s >= a) {
        decimal.mantissa = s;
        return remove_trailing_zeros(decimal);
      }

      else {
        m = mantissa2; // = 2 * binary.mantissa
        suint_t const c_hat = scale(upper, lower, n_bits, m);
        decimal.mantissa = c_hat / 2;
        if (decimal.mantissa < a)
          ++decimal.mantissa;
        else if (c_hat % 2 == 1)
          decimal.mantissa += decimal.mantissa % 2 == 1 ||
            !(e >= -(mantissa_size + 1) && decimal.exponent <= 0);
      }
    }
    else {
      --decimal.exponent;
      m = 20 * binary.mantissa; // = 20 * binary.mantissa
      suint_t const c_hat = scale(upper, lower, n_bits, m);
      decimal.mantissa = c_hat / 2;
      if (c_hat % 2 == 1)
        decimal.mantissa += decimal.mantissa % 2 == 1 ||
          !(e >= -(mantissa_size + 1) && decimal.exponent <= 0);
    }
  }
  return decimal;
}

#ifdef __cplusplus
}
#endif
