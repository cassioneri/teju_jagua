// gcc -include config32.h amaru.c common.o -o amaru

#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef AMARU_SIZE
  #error "Include config<SIZE>.h before amaru.h."
#endif

#include AMARU_TABLE

typedef struct {
  int        sign;
  int        exponent;
  AMARU_UINT mantissa;
} AMARU_REP;

static void to_decimal_small(AMARU_REP binary, AMARU_REP* decimal) {
}

static void to_decimal_medium(AMARU_REP binary, AMARU_REP* decimal) {
}

static void to_decimal_large(AMARU_REP binary, AMARU_REP* decimal) {

  decimal->sign     = binary.sign;
  decimal->exponent = log10_pow2(binary.exponent);

  AMARU_UBIGINT f   = params[binary.exponent - 37];
  AMARU_UINT    m   = 2*binary.mantissa + 1;
  __uint128_t   p   = ((__uint128_t) f) * m;
  AMARU_UINT    b   = p >> AMARU_SHIFT;
                p  -= 2*f;
  AMARU_UINT    a   = (p >> AMARU_SHIFT) + 1;
  AMARU_UINT    c   = (b/10)*10;

  if (a <= c) {
    printf("z1 ");
    decimal->mantissa = c;
  }
  else if (a % 2 == b % 2) {
    printf("m1 ");
    decimal->mantissa = (a + b)/2;
  }
  else {
    printf("a1 ");
    decimal->mantissa = (a + b)/2;
  }
}

static AMARU_UINT mask(uint32_t size) {
  return pow2(size) - 1;
}

static void value_to_ieee(AMARU_TYPE value, AMARU_REP* ieee) {

  AMARU_UINT uint;

  memcpy(&uint, &value, sizeof(value));

  ieee->mantissa = uint & mask(AMARU_MANTISSA_SIZE);
  uint >>= AMARU_MANTISSA_SIZE;

  ieee->exponent = uint & mask(AMARU_EXPONENT_SIZE);
  uint >>= AMARU_EXPONENT_SIZE;

  ieee->sign = uint ? -1 : 1;
}

static void ieee_to_value(AMARU_REP ieee, AMARU_TYPE* value) {

  AMARU_UINT uint = ieee.sign == 1 ? 0 : 1;

  uint <<= AMARU_EXPONENT_SIZE;
  uint |= ieee.exponent;

  uint <<= AMARU_MANTISSA_SIZE;
  uint |= ieee.mantissa;

  memcpy(value, &uint, sizeof(uint));
}

static void ieee_to_amaru(AMARU_REP ieee, AMARU_REP* amaru) {
  amaru->sign = ieee.sign;
  amaru->exponent = AMARU_SPEC.E0 + (ieee.exponent <= 1 ? 0 :
    ieee.exponent - 1);
  amaru->mantissa = ieee.mantissa + (ieee.exponent == 0 ? 0 :
    pow2(AMARU_SPEC.P));
}

static void amaru_to_ieee(AMARU_REP amaru, AMARU_REP* ieee) {
  ieee->mantissa = amaru.mantissa & (pow2(AMARU_SPEC.P) - 1);
  ieee->exponent = amaru.exponent - AMARU_SPEC.E0 +
    (amaru.mantissa >= pow2(AMARU_SPEC.P) ? 1 : 0);
  ieee->sign = amaru.sign;
}

void AMARU_TO_DECIMAL(AMARU_TYPE value, AMARU_REP* decimal) {

  AMARU_REP ieee, binary;

  value_to_ieee(value, &ieee);
  ieee_to_amaru(ieee, &binary);

  if (binary.exponent == AMARU_SPEC.E0 && binary.mantissa == 0) {
    decimal->exponent = 0;
    decimal->mantissa = 0;
  }

  else if (binary.exponent < 0)
    to_decimal_small(binary, decimal);

  else if (binary.exponent < 37)
    to_decimal_medium(binary, decimal);

  else
    to_decimal_large(binary, decimal);

  decimal->sign = binary.sign;
}

int main() {

//   AMARU_REP ieee, amaru;
//   float f = 1.f;
//
//   value_to_ieee(f, &ieee);
//   printf("sign = %d, exponent = %d, mantissa = %x.\n", ieee.sign,
//     ieee.exponent, ieee.mantissa);
//
//   ieee_to_amaru(ieee, &amaru);
//   printf("sign = %d, exponent = %d, mantissa = %x.\n", amaru.sign,
//     amaru.exponent, amaru.mantissa);
//
//   amaru_to_ieee(amaru, &ieee);
//   printf("sign = %d, exponent = %d, mantissa = %x.\n", ieee.sign,
//     ieee.exponent, ieee.mantissa);
//
//   ieee_to_value(ieee, &f);
//   printf("%.9e\n", f);

  AMARU_REP binary = { 1, 37, 8388608 };
  AMARU_REP ieee, decimal;
  AMARU_TYPE value;
  AMARU_UINT i_value;

  amaru_to_ieee(binary, &ieee);
  ieee_to_value(ieee, &value);

  do {

    to_decimal_large(binary, &decimal);

    printf("%d*2^%d:\t%.7e\t%d\n", binary.mantissa, binary.exponent, value, decimal.mantissa);
//     printf("%.9e\n", value);
//   printf("sign = %d, exponent = %d, mantissa = %d.\n", decimal.sign,
//     decimal.exponent, decimal.mantissa);

    memcpy(&i_value, &value, sizeof(value));
    ++i_value;
    memcpy(&value, &i_value, sizeof(value));

    value_to_ieee(value, &ieee);
    ieee_to_amaru(ieee, &binary);

  } while (ieee.exponent != 255);

  return 0;
}

