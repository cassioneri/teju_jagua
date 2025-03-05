// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/div10.h
 *
 * Different algorithms for division by 10.
 */

#ifndef TEJU_TEJU_DIV10_H_
#define TEJU_TEJU_DIV10_H_

#if !defined(teju_u1_t) || !defined(teju_calculation_shift) || \
  !defined(teju_size)
  #error "Macros teju_size and teju_u1_t must be defined prior to inclusion of div10.h."
#endif

#include "teju/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the quotient of the division by 10.
 *
 * @param m The dividend.
 *
 * @returns m / 10.
 */
static inline
teju_u1_t
teju_div10(teju_u1_t const m) {

  // TODO (CN): Assert that m is in the range of validity.

  #if !defined(teju_calculation_div10)

    return m / 10u;

  #elif teju_calculation_div10 == teju_built_in_2

    teju_u1_t const inv10 = ((teju_u1_t) -1) / 10u + 1u;
    return (((teju_u2_t) inv10) * m) >> teju_size;

  #elif teju_calculation_div10 == teju_synthetic_1

    teju_u1_t const inv10 = ((teju_u1_t) -1) / 10u + 1u;
    teju_u1_t upper;
    (void) teju_multiply(inv10, m, &upper);
    return upper;

  #elif teju_calculation_div10 == teju_built_in_1

    teju_u1_t const p2   = ((teju_u1_t) 1) << (teju_size / 2u);
    teju_u1_t const inv5 = (p2 - 1) / 5u;
    teju_u1_t const u    = m / p2;
    teju_u1_t const l    = m % p2;

    return (((l * (inv5 + 1)) / p2 + l * inv5 + u * (inv5 + 1)) / p2 +
      u * inv5) / 2u;

  #else

    #error "Invalid definition of macro teju_calculation_div10."

  #endif

}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_DIV10_H_
