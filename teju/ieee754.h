// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/ieee754.h
 *
 * Constants defined by the IEEE-754 standard and converters to Teju Jagua's
 * binary representation.
 */

#ifndef TEJU_TEJU_IEEE754_H_
#define TEJU_TEJU_IEEE754_H_

#include "teju/common.h"

// https://en.wikipedia.org/wiki/IEEE_754#Basic_and_interchange_formats
// Mantissa sizes exclude the implicit bit.

enum {

   teju_ieee754_binary16_exponent_size  =   5,
   teju_ieee754_binary16_mantissa_size  =  10,

   teju_ieee754_binary32_exponent_size  =   8,
   teju_ieee754_binary32_mantissa_size  =  23,

   teju_ieee754_binary64_exponent_size  =  11,
   teju_ieee754_binary64_mantissa_size  =  52,

   teju_ieee754_binary128_exponent_size =  15,
   teju_ieee754_binary128_mantissa_size = 112,

   teju_ieee754_binary256_exponent_size =  19,
   teju_ieee754_binary256_mantissa_size = 236,
};

#define teju_min_binary_exponent_from_ieee754(exponent_size, mantissa_size) \
  (-teju_pow2(int32_t, exponent_size - 1) - ((int32_t) mantissa_size) + 2)

#define teju_max_binary_exponent_from_ieee754(exponent_size, mantissa_size) \
  (teju_pow2(int32_t, exponent_size - 1) - ((int32_t) mantissa_size) - 1)

#endif // TEJU_TEJU_IEEE754_H_
