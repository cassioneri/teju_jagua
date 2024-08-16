// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/ryu.hpp
 *
 * Teju Jagua's wrapper around dragonbox used for testing and benchmarking.
 */

#ifndef TEJU_CPP_COMMON_RYU_HPP_
#define TEJU_CPP_COMMON_RYU_HPP_

#include "teju/config.h"

#include <ryu/ryu.h>

#if defined(__GNUC__) || defined(__clang__)
extern "C" {
#endif

typedef struct floating_decimal_32 {
  uint32_t mantissa;
  int32_t exponent;
} floating_decimal_32;

typedef struct floating_decimal_64 {
  uint64_t mantissa;
  int32_t exponent;
} floating_decimal_64;

floating_decimal_32 ryu_float_to_decimal(float f);
floating_decimal_64 ryu_double_to_decimal(double f);

#if defined(__GNUC__) || defined(__clang__)
} // extern "C"
#endif

#endif // TEJU_CPP_COMMON_RYU_HPP_
