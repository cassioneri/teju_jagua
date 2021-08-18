// gcc -I ~/ryu/cassio/ryu -include config32.h amaru.c common.o -o amaru

#include "common.h"

#include <ryu.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef AMARU_SIZE
  #error "Include config<SIZE>.h before amaru.h."
#endif

#include AMARU_TABLE

static inline uint32_t remove_trailing_zeros(AMARU_UINT* value) {
  uint32_t count = 0;
  do {
    ++count;
    *value /= 10;
  } while (*value % 10 == 0);
  return count;
}

typedef struct {
  int        sign;
  int        exponent;
  AMARU_UINT mantissa;
} AMARU_REP;

static inline AMARU_REP to_decimal_small(AMARU_REP const binary) {
  AMARU_REP decimal;
  return decimal;
}

static inline AMARU_REP to_decimal_medium(AMARU_REP const binary) {
  AMARU_REP decimal;
  return decimal;
}

static inline AMARU_REP to_decimal_large(AMARU_REP const binary) {

  AMARU_REP decimal;

  decimal.sign     = binary.sign;
  decimal.exponent = log10_pow2(binary.exponent);

  __uint128_t const f = params[binary.exponent - 37];
  AMARU_UINT  const m = 2*binary.mantissa + 1;
  __uint128_t       p = f*m;
  AMARU_UINT  const b = p >> AMARU_SHIFT;
                    p = p - 2*f;
  AMARU_UINT  const a = (p >> AMARU_SHIFT) + 1;
  AMARU_UINT        c = (b/10)*10;

  if (a <= c) {
    decimal.exponent += remove_trailing_zeros(&c);
    decimal.mantissa = c;
  }
  else if (a % 2 == b % 2)
    decimal.mantissa = (a + b)/2;
  else {
    p = 4*binary.mantissa*f;
    c = p >> AMARU_SHIFT;
    decimal.mantissa = (a + b)/2 + (c & 1);
  }
  return decimal;
}

static inline AMARU_UINT mask(uint32_t const size) {
  return pow2(size) - 1;
}

static inline AMARU_REP value_to_ieee(AMARU_TYPE const value) {

  AMARU_REP  ieee;
  AMARU_UINT uint;

  memcpy(&uint, &value, sizeof(value));

  ieee.mantissa = uint & mask(AMARU_MANTISSA_SIZE);
  uint >>= AMARU_MANTISSA_SIZE;

  ieee.exponent = uint & mask(AMARU_EXPONENT_SIZE);
  uint >>= AMARU_EXPONENT_SIZE;

  ieee.sign = uint ? -1 : 1;

  return ieee;
}

static inline AMARU_TYPE ieee_to_value(AMARU_REP const ieee) {

  AMARU_TYPE value;
  AMARU_UINT uint = ieee.sign == 1 ? 0 : 1;

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

  amaru.exponent = AMARU_SPEC.E0 + (ieee.exponent <= 1 ? 0 :
    ieee.exponent - 1);

  amaru.mantissa = ieee.mantissa + (ieee.exponent == 0 ? 0 :
    pow2(AMARU_SPEC.P));

  return amaru;
}

static inline AMARU_REP amaru_to_ieee(AMARU_REP const amaru) {

  AMARU_REP ieee;

  ieee.mantissa = amaru.mantissa & (pow2(AMARU_SPEC.P) - 1);

  ieee.exponent = amaru.exponent - AMARU_SPEC.E0 +
    (amaru.mantissa >= pow2(AMARU_SPEC.P) ? 1 : 0);

  ieee.sign = amaru.sign;

  return ieee;
}

static inline AMARU_REP AMARU_TO_DECIMAL(AMARU_TYPE value) {

  AMARU_REP decimal;
  AMARU_REP const ieee   = value_to_ieee(value);
  AMARU_REP const binary = ieee_to_amaru(ieee);

  if (binary.exponent == AMARU_SPEC.E0 && binary.mantissa == 0) {
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

  AMARU_REP binary = { 1, 37, 8388608 };
  AMARU_REP ieee, decimal;
  AMARU_TYPE value;
  AMARU_UINT i_value;

  floating_decimal_32 ryu;

  ieee  = amaru_to_ieee(binary);
  value = ieee_to_value(ieee);

  uint32_t result = 0;

#define AMARU_DO_RYU   1
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

