// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/teju.h
 *
 * The implementation of Teju Jagua and some of its helpers.
 */

#ifndef TEJU_TEJU_TEJU_H_
#define TEJU_TEJU_TEJU_H_

#define teju_static_assert(c, msg) \
do {                               \
  const int _ = (c) ? 1 : -1;      \
  int                              \
  static_assert_failed[_];         \
  (void) static_assert_failed;     \
} while(false)

#include "teju/common.h"
#include "teju/config.h"
#include "teju/div10.h"
#include "teju/mshift.h"

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks whether mantissa m is multiple of 2^e.
 *
 * @pre 0 <= e && e <= teju_mantissa_size.
 *
 * @param m                 The mantissa m.
 * @param e                 The exponent e.
 */
static inline
bool
is_multiple_of_pow2(teju_u1_t const m, int32_t const e) {
  return ((m >> e) << e) == m;
}

/**
 * @brief Checks whether the number m * 2^e is a small integer.
 *
 * @param m                 The mantissa m.
 * @param e                 The exponent e.
 */
static inline
bool
is_small_integer(teju_u1_t const m, int32_t const e) {
  return (-teju_mantissa_size <= e && e <= 0) && is_multiple_of_pow2(m, -e);
}

/**
 * @brief Checks whether mantissa m is multiple of 5^f.
 *
 * @pre minverse[f] is well defined.
 *
 * @param m                 The mantissa m.
 * @param f                 The exponent f.
 */
static inline
bool
is_multiple_of_pow5(teju_u1_t const m, int32_t const f) {
  return ((teju_u1_t) (m * minverse[f].multiplier)) <= minverse[f].bound;
}

/**
 * @brief Checks whether m, for m in { m_a, m_b, c_2 }, yields a tie.
 *
 * @param m                 The number m.
 * @param f                 The exponent f (for m == m_a and m == m_b) or
 *                          its negation -f for (m == c_2).
 */
static inline
bool
is_tie(teju_u1_t const m, int32_t const f) {
  return 0 <= f && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    is_multiple_of_pow5(m, f);
}

/**
 * @brief Checks whether the mantissa of an uncentred value (whose decimal
 * exponent is f) yields a tie.
 *
 * @param f                 The exponent f.
 */
static inline
bool
is_tie_uncentred(int32_t const f) {
  return f > 0 && teju_mantissa_size % 4u == 2u;
}

/**
 * @brief Creates teju_fields_t from exponent and mantissa.
 *
 * @param m                 The mantissa.
 * @param e                 The exponent.
 */
static inline
teju_fields_t
make_fields(teju_u1_t const m, int32_t const e) {
  teju_fields_t const fields = { m, e };
  return fields;
}

/**
 * @brief Rotates bits of a given number 1 position to the right.
 *
 * @param m                 The number.
 */
static inline
teju_u1_t ror(teju_u1_t m) {
  return m << (teju_size - 1u) | m >> 1u;
}

/**
 * @brief Shortens the decimal representation m\cdot 10^e\f by removing trailing
 * zeros of m and increasing e.
 *
 * @param m                 The mantissa m.
 * @param e                 The exponent e.
 *
 * @returns The fields of the shortest close decimal representation.
 */
static inline
teju_fields_t
remove_trailing_zeros(teju_u1_t m, int32_t e) {
  teju_u1_t const multiplier = minverse[1].multiplier;
  teju_u1_t const bound = minverse[1].bound / 2u;
  while (true) {
    teju_u1_t const q = ror(m * multiplier);
    if (q >= bound)
      return make_fields(m, e);
    ++e;
    m = q;
  }
}

/**
 * @brief Teju Jagua itself.
 *
 * Finds the shortest decimal representation of m * 2^e.
 *
 * @param e                 The exponent e.
 * @param m                 The mantissa m.
 *
 * @returns The fields of the shortest decimal representation.
 */
teju_fields_t
teju_function(teju_fields_t const binary) {

  int32_t     const e = binary.exponent;
  teju_u1_t const m = binary.mantissa;

  if (is_small_integer(m, e))
    return remove_trailing_zeros(m >> -e, 0);

  teju_u1_t const m_0 = teju_pow2(teju_u1_t, teju_mantissa_size);
  int32_t   const f   = teju_log10_pow2(e);
  uint32_t  const r   = teju_log10_pow2_residual(e);
  uint32_t  const i   = f - teju_storage_index_offset;
  teju_u1_t const u   = multipliers[i].upper;
  teju_u1_t const l   = multipliers[i].lower;

  if (m != m_0 || e == teju_exponent_minimum) {

    // Calculations of m_a, m_b and m_c are safe in the centred case if
    // teju_u1_t can hold m_c = 4 * m << r.
    bool const is_safe_centred = teju_mantissa_size + 6 <= teju_size;
    teju_static_assert(is_safe_centred, "Calculations might overflow.");

    teju_u1_t const m_a = (2u * m - 1u) << r;
    teju_u1_t const a   = teju_mshift(m_a, u, l);
    teju_u1_t const m_b = (2u * m + 1u) << r;
    teju_u1_t const b   = teju_mshift(m_b, u, l);
    teju_u1_t const q   = teju_div10(b);
    teju_u1_t const s   = 10u * q;

    if (s >= a) {
      if (s == b) {
        if (m % 2u == 0u || !is_tie(m_b, f))
          return remove_trailing_zeros(q, f + 1);
      }
      else if (s > a || (m % 2u == 0u && is_tie(m_a, f)))
        return remove_trailing_zeros(q, f + 1);
    }

    if ((a + b) % 2u == 1u)
      return make_fields((a + b) / 2u + 1u, f);

    teju_u1_t const m_c = 4u * m << r;
    teju_u1_t const c_2 = teju_mshift(m_c, u, l);
    teju_u1_t const c   = c_2 / 2u;

    if (c_2 % 2u == 0 || (c % 2u == 0 && is_tie(c_2, -f)))
      return make_fields(c, f);

    return make_fields(c + 1u, f);
  }

  // Calculations of m_a and m_b are safe in the uncentred case if
  // teju_u1_t can represent (4 * m_0 - 1) << r.
  bool const is_safe_uncentred = teju_mantissa_size + 6 <= teju_size;
  teju_static_assert(is_safe_uncentred, "Calculations might overflow.");

  teju_u1_t const m_a = (4u * m_0 - 1u) << r;
  teju_u1_t const a   = teju_mshift(m_a, u, l) / 2u;
  teju_u1_t const m_b = (2u * m_0 + 1u) << r;
  teju_u1_t const b   = teju_mshift(m_b, u, l);

  if (!teju_calculation_refine || b > a) {

    teju_u1_t const q = teju_div10(b);
    teju_u1_t const s = 10u * q;

    if (s > a || (s == a && is_tie_uncentred(f)))
      return remove_trailing_zeros(q, f + 1);

    // m_c = 4 * m_0 * 2^r = 2^{teju_mantissa_size + 2 + r}
    // c_2 = teju_mshift(m_c, upper, lower);
    uint32_t  const log2_m_c = teju_mantissa_size + 2u + r;
    teju_u1_t const c_2      = mshift_pow2(log2_m_c, u, l);
    teju_u1_t const c        = c_2 / 2u;

    if (c == a && !is_tie_uncentred(f))
      return make_fields(c + 1u, f);

    if (c_2 % 2u == 0 || (c % 2u == 0 && is_tie(c_2, -f)))
      return make_fields(c, f);

    return make_fields(c + 1u, f);
  }

  else if (is_tie_uncentred(f))
    return remove_trailing_zeros(a, f);

  // Calculation of m_c is safe in the refined uncentred case if
  // teju_u1_t can represent m_c = (40 * m_0 - 1) << r.
  bool const is_safe_uncentred_refined = !teju_calculation_refine ||
    teju_mantissa_size + 9 <= teju_size;
  teju_static_assert(is_safe_uncentred_refined, "Calculations might overflow.");

  teju_u1_t const m_c = 40u * m_0 << r;
  teju_u1_t const c_2 = teju_mshift(m_c, u, l);
  teju_u1_t const c   = c_2 / 2u;

  if (c_2 % 2u == 0 || (c % 2u == 0 && is_tie(c_2, -f)))
    return make_fields(c, f - 1);

  return make_fields(c + 1u, f - 1);

}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_TEJU_H_
