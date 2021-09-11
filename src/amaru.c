// gcc -O3 -I include -I ~/ryu/cassio/ryu src/amaru.c -o amaru ~/ryu/cassio/ryu/libryu.a -Wall -Wextra

#define AMARU_DO_RYU   1
#define AMARU_DO_AMARU 1

//-------------------------------------------------------------------------

#if AMARU_DO_RYU
  #include <ryu.h>
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "table32.h"

#define AMARU_MANTISSA_MIN  (((suint_t)1) << mantissa_size)
#define AMARU_LOG10_POW2(e) ((int32_t)(1292913987*((uint64_t) e) >> 32))

static inline
uint32_t remove_trailing_zeros(suint_t* value) {

  suint_t const m = (~((suint_t)0))/10 + 1;

  uint32_t count = 0;
  suint_t  x     = *value;
  duint_t  p     = ((duint_t) m)*x;
  suint_t  r     = (suint_t) p;

  while (r < m) {
    ++count;
    x = (suint_t) (p >> word_size);
    p = ((duint_t) m)*x;
    r = (suint_t) p;
  }
  *value = x;
  return count;
}

static inline
duint_t lower_bits(duint_t n, uint32_t n_bits) {
  return ((~((duint_t) 0)) >> (2*word_size - n_bits)) & n;
}

static inline
duint_t add(duint_t upper, duint_t lower) {
  return (((duint_t) upper) << word_size) + lower;
}

static inline
suint_t scale(suint_t const upper, suint_t const lower,
  uint32_t const n_bits, suint_t const m) {

  duint_t const upper_prod  = ((duint_t) upper)*m;
  duint_t const lower_prod  = ((duint_t) lower)*m;

  duint_t const upper_limbs = upper_prod + (lower_prod >> word_size);

  if (n_bits >= word_size)
    return upper_limbs >> (n_bits - word_size);

  suint_t const lower_limb  = (suint_t) lower_prod;
  return (lower_limb >> n_bits) | (upper_limbs << (word_size - n_bits));
}

static inline
suint_t is_multiple_of_pow5(suint_t const m, suint_t const upper,
  suint_t const lower, uint32_t const n_bits) {

  duint_t const lower_prod = ((duint_t) lower)*m;
  duint_t const upper_prod = ((duint_t) upper)*m;

  if (n_bits >= 2*word_size) {

    duint_t const upper_limbs = upper_prod + (lower_prod >> word_size);

    if (lower_bits(upper_limbs, n_bits - word_size) >> word_size > 0)
      return false;

    duint_t const lower_limb = (suint_t) lower_prod;
    duint_t const prod       = add(upper_limbs, lower_limb);
    duint_t const multiplier = add(upper, lower);
    return prod < multiplier;
  }

  duint_t const prod       = add(upper_prod, lower_prod);
  duint_t const multiplier = add(upper, lower);

  return lower_bits(prod, n_bits) < multiplier;
}

static inline
rep_t value_to_ieee(fp_t const value) {

  rep_t   ieee;
  suint_t uint;

  memcpy(&uint, &value, sizeof(value));

  ieee.mantissa = lower_bits(uint, mantissa_size);
  uint >>= mantissa_size;

  ieee.exponent = lower_bits(uint, exponent_size);
  uint >>= exponent_size;

  ieee.negative = uint;

  return ieee;
}

static inline
fp_t ieee_to_value(rep_t const ieee) {

  suint_t uint;
  uint   = ieee.negative;
  uint <<= exponent_size;
  uint  |= ieee.exponent;
  uint <<= mantissa_size;
  uint  |= ieee.mantissa;

  fp_t value;
  memcpy(&value, &uint, sizeof(uint));

  return value;
}

static inline
rep_t ieee_to_amaru(rep_t const ieee) {

  rep_t amaru;

  amaru.negative = ieee.negative;
  amaru.exponent = exponent_min +
    (ieee.exponent <= 1 ? 0 : ieee.exponent - 1);
  amaru.mantissa = ieee.mantissa +
    (ieee.exponent == 0 ? 0 : AMARU_MANTISSA_MIN);

  return amaru;
}

static inline
rep_t amaru_to_ieee(rep_t const amaru) {

  rep_t ieee;

  ieee.mantissa = lower_bits(amaru.mantissa, mantissa_size);
  ieee.exponent = amaru.exponent - exponent_min +
    (amaru.mantissa >= AMARU_MANTISSA_MIN ? 1 : 0);
  ieee.negative = amaru.negative;

  return ieee;
}

rep_t AMARU_TO_DECIMAL(fp_t value) {

  rep_t const ieee   = value_to_ieee(value);
  rep_t const binary = ieee_to_amaru(ieee);

  rep_t decimal;
  decimal.negative = binary.negative;

  if (binary.exponent == exponent_min && binary.mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
    return decimal;
  }

  decimal.exponent = AMARU_LOG10_POW2(binary.exponent);

  uint32_t const index     = binary.exponent + 149;
  suint_t  const upper     = scalers[index].upper;
  suint_t  const lower     = scalers[index].lower;
  uint32_t const n_bits    = scalers[index].n_bits;

  if (binary.mantissa != AMARU_MANTISSA_MIN) {

    bool    const check_mid = binary.exponent > 0 &&
      decimal.exponent <= large_exponent;
    suint_t       m = 2*binary.mantissa + 1;
    suint_t const b = scale(upper, lower, n_bits, m);
    suint_t const c = 10*(b/10);

    bool shorten;
    int32_t const e = binary.exponent - decimal.exponent - 1;

    if (c == b) {
      bool const is_mid = check_mid &&
         is_multiple_of_pow5(m, upper, lower, n_bits + e);
      shorten = !is_mid || binary.mantissa % 2 == 0;
    }

    else {

      m = m - 2; // = 2*binary.mantissa - 1;
      bool const is_mid = check_mid &&
        is_multiple_of_pow5(m, upper, lower, n_bits + e);
      suint_t const a = scale(upper, lower, n_bits, m) + !is_mid;
      shorten = c > a || (c == a && (!is_mid || binary.mantissa % 2 == 0));
    }

    if (shorten) {
      decimal.mantissa  = c;
      decimal.exponent += remove_trailing_zeros(&decimal.mantissa);
    }

    else {
      m = 4*binary.mantissa;
      suint_t const d = scale(upper, lower, n_bits, m);
      decimal.mantissa = d/2;
      if (d%2 == 1) {
        if (decimal.mantissa%2 == 1)
          decimal.mantissa += 1;
        else {
          const bool d_is_exact = 0 > e && e > -mantissa_size - 2 &&
            ((((((suint_t) 1) << -e) - 1) & m) == 0);
          decimal.mantissa += !d_is_exact;
        }
      }
    }
  }

  else {

    uint32_t const correction = correctors[index].correction;
    bool     const refine     = correctors[index].refine;
    suint_t  const m          = 2*binary.mantissa - 1;
    decimal.mantissa          = scale(upper, lower, n_bits, m);

    if (refine) {
      --decimal.exponent;
      decimal.mantissa *= 10;
    }

    decimal.mantissa += correction;
    decimal.exponent += remove_trailing_zeros(&decimal.mantissa);
  }

  return decimal;
}

int main() {

  uint32_t result = 0;
  int32_t  e2     = -149;
  rep_t    binary = { false, e2, 1 };
//  int32_t  e2     = -32;
//  rep_t    binary = { false, e2, AMARU_MANTISSA_MIN}; // AMARU_MANTISSA_MIN
  rep_t    ieee   = amaru_to_ieee(binary);
  fp_t     value  = ieee_to_value(ieee);

  printf("%d\n", e2);

  while (isfinite(value)) {

    #if AMARU_DO_RYU
      ieee = value_to_ieee(value);
      floating_decimal_32 ryu = f2d(ieee.mantissa, ieee.exponent);
      result += ryu.mantissa;
    #endif

    #if AMARU_DO_AMARU
      rep_t decimal = AMARU_TO_DECIMAL(value);
      result += decimal.mantissa;
    #endif

    #if AMARU_DO_RYU && AMARU_DO_AMARU
      binary = ieee_to_amaru(ieee);
      if (binary.exponent != e2) {
        e2 = binary.exponent;
        printf("%d\n", e2);
      }
      if ((ryu.mantissa != decimal.mantissa || ryu.exponent != decimal.exponent))
        printf("%d*2^%d:\t%.7e\t%d %d\t%d %d\n", binary.mantissa, binary.exponent, value, ryu.mantissa, ryu.exponent, decimal.mantissa, decimal.exponent);
    #endif

    suint_t i_value;
    memcpy(&i_value, &value, sizeof(value));
    ++i_value;
    memcpy(&value, &i_value, sizeof(value));
  }

  return result;
}
