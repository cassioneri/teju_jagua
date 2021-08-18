// gcc -O3 -I ~/ryu/cassio/ryu -include config32.h amaru.c -o amaru ~/ryu/cassio/ryu/libryu.a

#include <ryu.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include AMARU_TABLE

#define AMARU_E0 \
  (-(1u << (AMARU_EXPONENT_SIZE - 1)) - AMARU_MANTISSA_SIZE + 2)

#define AMARU_P2_MANTISSA_SIZE \
  (((AMARU_SINGLE) 1) << AMARU_MANTISSA_SIZE)

inline static int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (((uint64_t) 1292913986) * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}

static inline unsigned remove_trailing_zeros(AMARU_SINGLE* value) {
  unsigned count = 0;
  do {
    ++count;
    *value /= 10;
  } while (*value % 10 == 0);
  return count;
}

typedef struct {
  int          sign;
  int          exponent;
  AMARU_SINGLE mantissa;
} AMARU_REP;

static inline AMARU_REP to_decimal_small(AMARU_REP const binary) {
  AMARU_REP decimal;
  return decimal;
}

static inline AMARU_REP to_decimal_medium(AMARU_REP const binary) {
  AMARU_REP decimal;
  return decimal;
}

static inline AMARU_SINGLE
scale_mantissa(AMARU_DOUBLE const a, AMARU_SINGLE const x) {
  unsigned     const n  = 8*sizeof(AMARU_SINGLE);
  AMARU_DOUBLE const al = (AMARU_SINGLE) a;
  AMARU_DOUBLE const ah = a >> n;
  return ((al*x >> n) + ah*x) >> (AMARU_SHIFT - n);
}

static inline AMARU_REP to_decimal_large(AMARU_REP const binary) {

  AMARU_REP decimal;

  decimal.sign         = binary.sign;
  decimal.exponent     = log10_pow2(binary.exponent);

  AMARU_DOUBLE const f = params[binary.exponent - 37];

  AMARU_SINGLE const a = scale_mantissa(f, 2*binary.mantissa - 1) + 1;
  AMARU_SINGLE const b = scale_mantissa(f, 2*binary.mantissa + 1);

  AMARU_SINGLE c = 10*(b/10);
  AMARU_SINGLE d;

  if (a <= c) {
    decimal.exponent += remove_trailing_zeros(&c);
    decimal.mantissa = c;
  }
  else if (a % 2 == b % 2)
    decimal.mantissa = (a + b)/2;
  else {
    d = scale_mantissa(f, 4*binary.mantissa);
    decimal.mantissa = (a + b)/2 + (d & 1);
  }
  return decimal;
}

static inline AMARU_REP value_to_ieee(AMARU_FLOAT const value) {

  AMARU_REP  ieee;
  AMARU_SINGLE uint;

  memcpy(&uint, &value, sizeof(value));

  ieee.mantissa = uint & (AMARU_P2_MANTISSA_SIZE - 1);
  uint >>= AMARU_MANTISSA_SIZE;

  ieee.exponent = uint & (AMARU_P2_MANTISSA_SIZE - 1);
  uint >>= AMARU_EXPONENT_SIZE;

  ieee.sign = uint ? -1 : 1;

  return ieee;
}

static inline AMARU_FLOAT ieee_to_value(AMARU_REP const ieee) {

  AMARU_FLOAT value;
  AMARU_SINGLE uint = ieee.sign == 1 ? 0 : 1;

  uint <<= AMARU_EXPONENT_SIZE;
  uint |= ieee.exponent;

  uint <<= AMARU_MANTISSA_SIZE;
  uint |= ieee.mantissa;

  memcpy(&value, &uint, sizeof(uint));

  return value;
}

static inline AMARU_REP ieee_to_amaru(AMARU_REP const ieee) {

  AMARU_REP amaru;

  amaru.sign = ieee.sign;

  amaru.exponent = AMARU_E0 + (ieee.exponent <= 1 ? 0 :
    ieee.exponent - 1);

  amaru.mantissa = ieee.mantissa + (ieee.exponent == 0 ? 0 :
    AMARU_P2_MANTISSA_SIZE);

  return amaru;
}

static inline AMARU_REP amaru_to_ieee(AMARU_REP const amaru) {

  AMARU_REP ieee;

  ieee.mantissa = amaru.mantissa & (AMARU_P2_MANTISSA_SIZE - 1);

  ieee.exponent = amaru.exponent - AMARU_E0 +
    (amaru.mantissa >= AMARU_P2_MANTISSA_SIZE ? 1 : 0);

  ieee.sign = amaru.sign;

  return ieee;
}

static inline AMARU_REP AMARU_TO_DECIMAL(AMARU_FLOAT value) {

  AMARU_REP decimal;
  AMARU_REP const ieee   = value_to_ieee(value);
  AMARU_REP const binary = ieee_to_amaru(ieee);

  if (binary.exponent == AMARU_E0 && binary.mantissa == 0) {
    decimal.exponent = 0;
    decimal.mantissa = 0;
  }

  else if (binary.exponent < 0)
    decimal = to_decimal_small(binary);

  else if (binary.exponent < 37)
    decimal = to_decimal_medium(binary);

  else
    decimal = to_decimal_large(binary);

  decimal.sign = binary.sign;

  return decimal;
}

int main() {

  AMARU_REP    binary = { 1, 37, 8388608 };
  AMARU_REP    ieee, decimal;
  AMARU_FLOAT  value;
  AMARU_SINGLE i_value;

  floating_decimal_32 ryu;

  ieee  = amaru_to_ieee(binary);
  value = ieee_to_value(ieee);

  uint32_t result = 0;

#define AMARU_DO_RYU   0
#define AMARU_DO_AMARU 1

  do {

    #if AMARU_DO_RYU
      ryu = f2d(ieee.mantissa, ieee.exponent);
      result += ryu.mantissa;
    #endif

    #if AMARU_DO_AMARU
      decimal = to_decimal_large(binary);
      result += decimal.mantissa;
    #endif

    #if AMARU_DO_RYU && AMARU_DO_AMARU
      if (ryu.mantissa != decimal.mantissa || ryu.exponent != decimal.exponent)
        printf("%d*2^%d:\t%.7e\t%d %d\t%d %d\n", binary.mantissa, binary.exponent, value, ryu.mantissa, ryu.exponent, decimal.mantissa, decimal.exponent);
    #endif

    memcpy(&i_value, &value, sizeof(value));
    ++i_value;
    memcpy(&value, &i_value, sizeof(value));

    ieee = value_to_ieee(value);

    #if AMARU_DO_AMARU
      binary = ieee_to_amaru(ieee);
    #endif

  } while (ieee.exponent != 255);

  return result;
}

