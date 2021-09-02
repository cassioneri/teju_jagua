// gcc -O3 -I include -I ~/ryu/cassio/ryu -include config32.h src/amaru.c -o amaru ~/ryu/cassio/ryu/libryu.a -Wall -Wextra

#define AMARU_DO_RYU   1
#define AMARU_DO_AMARU 1

//-------------------------------------------------------------------------

#if AMARU_DO_RYU
  #include <ryu.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include AMARU_TABLE

#define AMARU_E0 \
  (-(1 << (AMARU_EXPONENT_SIZE - 1)) - AMARU_MANTISSA_SIZE + 2)

#define AMARU_P2_MANTISSA_SIZE \
  (((unsigned) 1) << AMARU_MANTISSA_SIZE)

typedef struct {
  bool              negative;
  int               exponent;
  AMARU_UINT_SINGLE mantissa;
} AMARU_REP;

inline static
int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1292913986 * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}

inline static
int log5_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1849741732 * ((uint64_t) exponent) >> 32) :
    log5_pow2(-exponent) - 1;
}

static inline
unsigned remove_trailing_zeros(AMARU_UINT_SINGLE* value) {
  unsigned count = 0;
  while (*value % 10 == 0) {
    ++count;
    *value /= 10;
  }
  return count;
}

// static inline
// AMARU_REP to_decimal_small(AMARU_REP const binary) {
//   AMARU_REP decimal;
//   return decimal;
// }

// static inline
// AMARU_REP to_decimal_medium(AMARU_REP const binary) {
//   AMARU_REP decimal;
//   return decimal;
// }

static inline
AMARU_UINT_SINGLE scale(AMARU_UINT_SINGLE const high,
  AMARU_UINT_SINGLE const low, unsigned shift, AMARU_UINT_SINGLE const x) {
  unsigned const n  = 8*sizeof(AMARU_UINT_SINGLE);
  AMARU_UINT_DOUBLE y = x;
  return ((low*y >> n) + high*y) >> (shift - n);
}

static inline
AMARU_REP to_decimal_large(AMARU_REP const binary) {

  AMARU_REP decimal;

  decimal.negative = binary.negative;
  decimal.exponent = log10_pow2(binary.exponent);

  unsigned          const index = binary.exponent - 4;
  AMARU_UINT_SINGLE const high  = converters[index].high;
  AMARU_UINT_SINGLE const low   = converters[index].low;
  unsigned          const shift = converters[index].shift;
  AMARU_UINT_SINGLE const a     = scale(high, low, shift,
    2*binary.mantissa - 1) + 1;

  if (binary.mantissa != AMARU_P2_MANTISSA_SIZE) {

    AMARU_UINT_SINGLE const b = scale(high, low, shift,
      2*binary.mantissa + 1);

    AMARU_UINT_SINGLE const c = 10*(b/10);

    if (a <= c) {
      decimal.mantissa = c;
      decimal.exponent += remove_trailing_zeros(&decimal.mantissa);
    }

    else {
      decimal.mantissa = (a + b)/2;
      if ((a ^ b) & 1) {
        AMARU_UINT_SINGLE const d = scale(high, low, shift,
          4*binary.mantissa);
        decimal.mantissa += d % 2;
        return decimal;
      }
    }
  }

  else {

    decimal.mantissa = a;
    unsigned const correction = correctors[index].correction;
    bool     const refine     = correctors[index].refine;
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
AMARU_REP value_to_ieee(AMARU_FP const value) {

  AMARU_REP         ieee;
  AMARU_UINT_SINGLE uint;

  memcpy(&uint, &value, sizeof(value));

  ieee.mantissa = uint & (AMARU_P2_MANTISSA_SIZE - 1);
  uint >>= AMARU_MANTISSA_SIZE;

  ieee.exponent = uint & (AMARU_P2_MANTISSA_SIZE - 1);
  uint >>= AMARU_EXPONENT_SIZE;

  ieee.negative = uint;

  return ieee;
}

static inline
AMARU_FP ieee_to_value(AMARU_REP const ieee) {

  AMARU_FP          value;
  AMARU_UINT_SINGLE uint = ieee.negative;

  uint <<= AMARU_EXPONENT_SIZE;
  uint |= ieee.exponent;

  uint <<= AMARU_MANTISSA_SIZE;
  uint |= ieee.mantissa;

  memcpy(&value, &uint, sizeof(uint));

  return value;
}

static inline
AMARU_REP ieee_to_amaru(AMARU_REP const ieee) {

  AMARU_REP amaru;

  amaru.negative = ieee.negative;

  amaru.exponent = AMARU_E0 + (ieee.exponent <= 1 ? 0 :
    ieee.exponent - 1);

  amaru.mantissa = ieee.mantissa + (ieee.exponent == 0 ? 0 :
    AMARU_P2_MANTISSA_SIZE);

  return amaru;
}

static inline
AMARU_REP amaru_to_ieee(AMARU_REP const amaru) {

  AMARU_REP ieee;

  ieee.mantissa = amaru.mantissa & (AMARU_P2_MANTISSA_SIZE - 1);

  ieee.exponent = amaru.exponent - AMARU_E0 +
    (amaru.mantissa >= AMARU_P2_MANTISSA_SIZE ? 1 : 0);

  ieee.negative = amaru.negative;

  return ieee;
}

static inline
AMARU_REP AMARU_TO_DECIMAL(AMARU_FP value) {

  AMARU_REP decimal;
  AMARU_REP const ieee   = value_to_ieee(value);
  AMARU_REP const binary = ieee_to_amaru(ieee);

  if (binary.exponent == AMARU_E0 && binary.mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
  }

//   else if (binary.exponent < 0)
//     decimal = to_decimal_small(binary);
//
//   else if (binary.exponent < AMARU_LARGE)
//     decimal = to_decimal_medium(binary);

  else if (binary.exponent > AMARU_LARGE)
    decimal = to_decimal_large(binary);

  decimal.negative = binary.negative;

  return decimal;
}

int main() {

  AMARU_REP binary = { false, 37, AMARU_P2_MANTISSA_SIZE };
  AMARU_REP ieee   = amaru_to_ieee(binary);
  AMARU_FP  value  = ieee_to_value(ieee);

  unsigned result = 0;

  do {

    #if AMARU_DO_RYU
      floating_decimal_32 ryu = f2d(ieee.mantissa, ieee.exponent);
      result += ryu.mantissa;
    #endif

    #if AMARU_DO_AMARU
      AMARU_REP decimal = to_decimal_large(binary);
      result += decimal.mantissa;
    #endif

    #if AMARU_DO_RYU && AMARU_DO_AMARU
      if (ryu.mantissa != decimal.mantissa || ryu.exponent != decimal.exponent)
        printf("%d*2^%d:\t%.7e\t%d %d\t%d %d\n", binary.mantissa, binary.exponent, value, ryu.mantissa, ryu.exponent, decimal.mantissa, decimal.exponent);
    #endif

    AMARU_UINT_SINGLE i_value;
    memcpy(&i_value, &value, sizeof(value));
    ++i_value;
    memcpy(&value, &i_value, sizeof(value));

    ieee = value_to_ieee(value);

    #if AMARU_DO_AMARU
      binary = ieee_to_amaru(ieee);
    #endif

  } while (ieee.exponent != 255);

  printf("%.7e\n", value);

  return result;
}
