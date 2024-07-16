// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

#include "teju/common.h"
#include "teju/float.h"
#include "teju/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Conversion from IEEE-754's parameters to Teju Jagua's.
enum {
  exponent_size = teju_ieee754_binary32_exponent_size,
  mantissa_size = teju_ieee754_binary32_mantissa_size,
  exponent_min  = teju_ieee754_binary32_exponent_min - mantissa_size,
};

teju32_fields_t
teju_float_to_ieee32(float const value) {

  uint32_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju32_fields_t binary;
  binary.mantissa = teju_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t) teju_lsb(bits, exponent_size);

  return binary;
}

teju32_fields_t
teju_ieee32_to_binary(teju32_fields_t ieee32) {

  int32_t  e = ieee32.exponent + exponent_min;
  uint32_t m = ieee32.mantissa;

  if (ieee32.exponent != 0) {
    e -= 1;
    m += teju_pow2(uint64_t, mantissa_size);
  }

  teju32_fields_t teju_binary = { e, m };
  return teju_binary;
}

teju32_fields_t
teju_float_to_decimal(float const value) {
  teju32_fields_t ieee32      = teju_float_to_ieee32(value);
  teju32_fields_t teju_binary = teju_ieee32_to_binary(ieee32);
  return teju_ieee32(teju_binary);
}

#ifdef __cplusplus
}
#endif
