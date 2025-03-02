// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "teju/config.h"

#if defined(teju_has_float16)

#include "teju/common.h"
#include "teju/float16.h"
#include "teju/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Conversion from IEEE-754's parameters to Teju Jagua's.
enum {
  exponent_size = teju_ieee754_binary16_exponent_size,
  mantissa_size = teju_ieee754_binary16_mantissa_size,
  exponent_min  = teju_ieee754_binary16_exponent_min - mantissa_size,
};

teju16_fields_t
teju_float16_to_ieee16(float16_t const value) {

  uint16_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju16_fields_t binary;
  binary.mantissa = teju_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t) teju_lsb(bits, exponent_size);

  return binary;
}

teju16_fields_t
teju_ieee16_to_binary(teju16_fields_t ieee16) {

  int32_t  e = ieee16.exponent + exponent_min;
  uint16_t m = ieee16.mantissa;

  if (ieee16.exponent != 0) {
    e -= 1;
    m += teju_pow2(uint16_t, mantissa_size);
  }

  teju16_fields_t teju_binary = { m, e };
  return teju_binary;
}

teju16_fields_t
teju_float16_to_decimal(float16_t const value) {
  teju16_fields_t ieee16      = teju_float16_to_ieee16(value);
  teju16_fields_t teju_binary = teju_ieee16_to_binary(ieee16);
  return teju_ieee16(teju_binary);
}

#ifdef __cplusplus
}
#endif

# endif // defined(teju_has_float16)
