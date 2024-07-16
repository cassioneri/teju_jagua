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

#ifdef __cplusplus
extern "C" {
#endif

teju32_fields_t ryu_float_to_decimal(float f);
teju64_fields_t ryu_double_to_decimal(double d);

#ifdef __cplusplus
}
#endif

#endif // TEJU_CPP_COMMON_RYU_HPP_
