// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file amaru/ieee754.h
 *
 * Constants defined by the IEEE-754 standard and converters to Amaru's binary
 * representation.
 */

#ifndef AMARU_AMARU_IEEE754_H_
#define AMARU_AMARU_IEEE754_H_

#include "amaru/common.h"

// https://en.wikipedia.org/wiki/IEEE_754#Basic_and_interchange_formats
// Mantissa sizes exclude the implicit bit.

enum {

   amaru_ieee754_binary16_exponent_size  =   5,
   amaru_ieee754_binary16_mantissa_size  =  10,

   amaru_ieee754_binary32_exponent_size  =   8,
   amaru_ieee754_binary32_mantissa_size  =  23,

   amaru_ieee754_binary64_exponent_size  =  11,
   amaru_ieee754_binary64_mantissa_size  =  52,

   amaru_ieee754_binary128_exponent_size =  15,
   amaru_ieee754_binary128_mantissa_size = 112,

   amaru_ieee754_binary256_exponent_size =  19,
   amaru_ieee754_binary256_mantissa_size = 236,
};

#define amaru_min_binary_exponent_from_ieee754(exponent_size, mantissa_size)  \
  (-amaru_pow2(int32_t, exponent_size - 1) - ((int32_t) mantissa_size) + 2)

#define amaru_max_binary_exponent_from_ieee754(exponent_size, mantissa_size)  \
  (amaru_pow2(int32_t, exponent_size - 1) - ((int32_t) mantissa_size) - 1)

#endif // AMARU_AMARU_IEEE754_H_
