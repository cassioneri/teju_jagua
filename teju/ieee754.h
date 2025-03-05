// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/ieee754.h
 *
 * Constants defined by the IEEE-754 standard.
 *
 * https://en.wikipedia.org/wiki/IEEE_754#Basic_and_interchange_formats
 */

#ifndef TEJU_TEJU_IEEE754_H_
#define TEJU_TEJU_IEEE754_H_

enum {

  teju_ieee754_binary16_exponent_size  =       5u,
  teju_ieee754_binary16_mantissa_size  =      10u,
  teju_ieee754_binary16_exponent_min   =     -14,
  teju_ieee754_binary16_exponent_max   =      15,

  teju_ieee754_binary32_exponent_size  =       8u,
  teju_ieee754_binary32_mantissa_size  =      23u,
  teju_ieee754_binary32_exponent_min   =    -126,
  teju_ieee754_binary32_exponent_max   =     127,

  teju_ieee754_binary64_exponent_size  =      11u,
  teju_ieee754_binary64_mantissa_size  =      52u,
  teju_ieee754_binary64_exponent_min   =   -1022,
  teju_ieee754_binary64_exponent_max   =    1023,

  teju_ieee754_binary128_exponent_size =      15u,
  teju_ieee754_binary128_mantissa_size =     112u,
  teju_ieee754_binary128_exponent_min  =  -16382,
  teju_ieee754_binary128_exponent_max  =   16383,

  teju_ieee754_binary256_exponent_size =      19u,
  teju_ieee754_binary256_mantissa_size =     236u,
  teju_ieee754_binary256_exponent_min  = -262142,
  teju_ieee754_binary256_exponent_max  =  262143,
};

#endif // TEJU_TEJU_IEEE754_H_
