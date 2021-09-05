// gcc -O3 -I include -I ~/ryu/cassio/ryu -include config32.h src/amaru.c -o amaru ~/ryu/cassio/ryu/libryu.a -Wall -Wextra

#define AMARU_DO_RYU   1
#define AMARU_DO_AMARU 1

//-------------------------------------------------------------------------

#if AMARU_DO_RYU
  #include <ryu.h>
#endif

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include AMARU_TABLE

typedef AMARU_FP    fp_t;
typedef AMARU_SUINT suint_t;
typedef AMARU_DUINT duint_t;

typedef struct {
  bool   negative;
  int    exponent;
  suint_t mantissa;
} AMARU_REP;

typedef AMARU_REP rep_t;

static unsigned const exponent_size      = AMARU_EXPONENT_SIZE;
static unsigned const mantissa_size      = AMARU_MANTISSA_SIZE;
static int      const large_exponent     = AMARU_LARGE_EXPONENT;
static suint_t  const pow2_mantissa_size = ((suint_t) 1) << mantissa_size;
static unsigned const word_size          = CHAR_BIT*sizeof(suint_t);
static int      const E0                 =
  -(1 << (exponent_size - 1)) - mantissa_size + 2;

#undef AMARU_EXPONENT_SIZE
#undef AMARU_MANTISSA_SIZE
#undef AMARU_LARGE_EXPONENT
#undef AMARU_FP
#undef AMARU_SUINT
#undef AMARU_DUINT
#undef AMARU_REP
#undef AMARU_TO_DECIMAL
#undef AMARU_TABLE

static inline
int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1292913986 * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}

static inline
int log5_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1849741732 * ((uint64_t) exponent) >> 32) :
    log5_pow2(-exponent) - 1;
}

static inline
unsigned remove_trailing_zeros(suint_t* value) {
  unsigned count = 0;
  while (*value % 10 == 0) {
    ++count;
    *value /= 10;
  }
  return count;
}

static inline
duint_t lower_bits(duint_t n, unsigned n_bits) {
  return ((~((duint_t) 0)) >> (2*word_size - n_bits)) & n;
}

static inline
duint_t add(duint_t upper, duint_t lower) {
  return (((duint_t) upper) << word_size) + lower;
}

static inline
suint_t scale(suint_t const upper, suint_t const lower, unsigned n_bits,
  suint_t const m) {

  unsigned const n   = 8*sizeof(suint_t);

  duint_t  const pl  = ((duint_t) lower)*m;
  duint_t  const pmu = ((duint_t) upper)*m + (pl >> n);

  suint_t const x[] = {
    (suint_t) pl,
    (suint_t) pmu,
    (suint_t) (pmu >> n)
  };

  suint_t const *y = x + n_bits / n;
  n_bits = n_bits % n;

  return (y[0] >> n_bits) + (y[1] << (n - n_bits));
}

static inline
suint_t is_multiple_of_pow5(suint_t const upper, suint_t const lower,
  unsigned const n_bits, suint_t const m) {

  duint_t const lower_product = ((duint_t) lower)*m;
  duint_t const upper_product = ((duint_t) upper)*m;

  if (n_bits >= 2*word_size) {

    // Product has 3 limbs of size word_size.

    duint_t const upper_limbs = upper_product + (lower_product >> word_size);

    if (lower_bits(upper_limbs, n_bits - word_size) >> word_size > 0)
      return false;

    duint_t const lower_limb = (suint_t) lower_product;
    duint_t const product    = add(upper_limbs, lower_limb);
    duint_t const multiplier = add(upper, lower);
    return product < multiplier;
  }

  duint_t const product    = add(upper_product, lower_product);
  duint_t const multiplier = add(upper, lower);

  return lower_bits(product, n_bits) < multiplier;
}

static inline
rep_t to_decimal_mantissa_is_value(rep_t const binary) {
  rep_t decimal;
  decimal.negative  = binary.negative;
  decimal.exponent  = 0;
  decimal.mantissa  = binary.mantissa << binary.exponent;
  decimal.exponent += remove_trailing_zeros(&decimal.mantissa);
  return decimal;
}

static inline
rep_t to_decimal_positive(rep_t const binary, bool const check_mid) {

  rep_t decimal;

  decimal.negative = binary.negative;
  decimal.exponent = log10_pow2(binary.exponent);

  unsigned const index  = binary.exponent - 1;
  suint_t  const upper  = converters[index].upper;
  suint_t  const lower  = converters[index].lower;
  unsigned const n_bits = converters[index].n_bits;

  if (binary.mantissa != pow2_mantissa_size) {

    suint_t       m = 2*binary.mantissa + 1;
    suint_t const b = scale(upper, lower, n_bits, m);
    suint_t const c = 10*(b/10);

    bool shorten;
    unsigned const E = binary.exponent - 1 - decimal.exponent;

    if (c == b) {
      bool const is_mid = check_mid &&
         is_multiple_of_pow5(upper, lower, n_bits + E, m);
      shorten = !is_mid || binary.mantissa % 2 == 0;
    }

    else {

      m -= 2;
      bool const is_mid = check_mid &&
        is_multiple_of_pow5(upper, lower, n_bits + E, m);
      suint_t const a = scale(upper, lower, n_bits, m) + !is_mid;

      shorten = c > a || (c == a && (!is_mid || binary.mantissa % 2 == 0));
    }

    if (shorten) {
      decimal.mantissa  = c;
      decimal.exponent += remove_trailing_zeros(&decimal.mantissa);
    }

    else {
      suint_t const d  = scale(upper, lower, n_bits, 4*binary.mantissa);
      decimal.mantissa = (d + 1) / 2;
    }
  }

  else {

    unsigned const correction = correctors[index].correction;
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

static inline
rep_t value_to_ieee(fp_t const value) {

  rep_t   ieee;
  suint_t uint;

  memcpy(&uint, &value, sizeof(value));

  ieee.mantissa = uint & (pow2_mantissa_size - 1);
  uint >>= mantissa_size;

  ieee.exponent = uint & (pow2_mantissa_size - 1);
  uint >>= exponent_size;

  ieee.negative = uint;

  return ieee;
}

static inline
fp_t ieee_to_value(rep_t const ieee) {

  fp_t    value;
  suint_t uint = ieee.negative;

  uint <<= exponent_size;
  uint |= ieee.exponent;

  uint <<= mantissa_size;
  uint |= ieee.mantissa;

  memcpy(&value, &uint, sizeof(uint));

  return value;
}

static inline
rep_t ieee_to_amaru(rep_t const ieee) {

  rep_t amaru;

  amaru.negative = ieee.negative;

  amaru.exponent = E0 + (ieee.exponent <= 1 ? 0 : ieee.exponent - 1);

  amaru.mantissa = ieee.mantissa + (ieee.exponent == 0 ? 0 :
    pow2_mantissa_size);

  return amaru;
}

static inline
rep_t amaru_to_ieee(rep_t const amaru) {

  rep_t ieee;

  ieee.mantissa = amaru.mantissa & (pow2_mantissa_size - 1);

  ieee.exponent = amaru.exponent - E0 +
    (amaru.mantissa >= pow2_mantissa_size ? 1 : 0);

  ieee.negative = amaru.negative;

  return ieee;
}

static inline
rep_t AMARU_TO_DECIMAL(fp_t value) {

  rep_t decimal;
  rep_t const ieee   = value_to_ieee(value);
  rep_t const binary = ieee_to_amaru(ieee);

  if (binary.exponent == E0 && binary.mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
  }

//   else if (binary.exponent < 0)
//    decimal = to_decimal_negative(binary);

  else if (binary.exponent < large_exponent)
    decimal = to_decimal_positive(binary, true);

  else
    decimal = to_decimal_positive(binary, false);

  decimal.negative = binary.negative;

  return decimal;
}

int main() {

  unsigned result = 0;
  int      e2     = 1;
  rep_t    binary = { false, e2, pow2_mantissa_size };
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
      if (ryu.mantissa != decimal.mantissa || ryu.exponent != decimal.exponent)
        printf("%d*2^%d:\t%.7e\t%d %d\t%d %d\n", binary.mantissa, binary.exponent, value, ryu.mantissa, ryu.exponent, decimal.mantissa, decimal.exponent);
    #endif

    suint_t i_value;
    memcpy(&i_value, &value, sizeof(value));
    ++i_value;
    memcpy(&value, &i_value, sizeof(value));
  }

  return result;
}
