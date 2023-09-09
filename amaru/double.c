#include "amaru/common.h"
#include "amaru/double.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  mantissa_size =    52,
  exponent_size =    11,
  exponent_min  = -1074
};

amaru64_fields_t
amaru_to_ieee64(double const value) {

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  amaru64_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = amaru_lsb(bits, exponent_size);

  return binary;
}

amaru64_fields_t
amaru_ieee64_to_amaru(amaru64_fields_t ieee64) {

  amaru64_fields_t amaru_binary = ieee64;

  amaru_binary.exponent += exponent_min;
  if (ieee64.exponent != 0) {
    amaru_binary.mantissa += amaru_pow2(uint64_t, mantissa_size);
    amaru_binary.exponent -= 1;
  }

  return amaru_binary;
}

amaru64_fields_t
amaru_double_compact(double const value) {
  amaru64_fields_t ieee64       = amaru_to_ieee64(value);
  amaru64_fields_t amaru_binary = amaru_ieee64_to_amaru(ieee64);
  return amaru_ieee64_compact(amaru_binary);
}

amaru64_fields_t
amaru_double_full(double const value) {
  amaru64_fields_t ieee64       = amaru_to_ieee64(value);
  amaru64_fields_t amaru_binary = amaru_ieee64_to_amaru(ieee64);
  return amaru_ieee64_full(amaru_binary);
}

#ifdef __cplusplus
}
#endif
