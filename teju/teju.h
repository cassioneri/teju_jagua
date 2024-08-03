// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/teju.h
 *
 * The implementation of Teju Jagua and some of its helpers.
 */

#ifndef TEJU_TEJU_TEJU_H_
#define TEJU_TEJU_TEJU_H_

#include "teju/common.h"
#include "teju/config.h"
#include "teju/div10.h"
#include "teju/mshift.h"

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

#include <stdbool.h>
#include <stdint.h>

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
  return m * minverse[f].multiplier <= minverse[f].bound;
}

/**
 * @brief Checks whether m, for m in { m_a, m_b, c_ 2}, yields a tie.
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
 * @brief Checks whether m_a = 4 * m_0 - 1, where m_0 is the mantissa of
 * uncentred values, can yield a tie.
 *
 * @param f                 The exponent f.
 */
static inline
bool
is_tie_uncentred(int32_t const f) {
  return f > 0 && teju_mantissa_size % 4 == 2;
}

/**
 * @brief Creates teju_fields_t from exponent and mantissa.
 *
 * @param e                 The exponent.
 * @param m                 The mantissa.
 */
static inline
teju_fields_t
make_fields(int32_t const e, teju_u1_t const m) {
  teju_fields_t const fields = { e, m };
  return fields;
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
remove_trailing_zeros(teju_u1_t const m, int32_t const e) {
  teju_u1_t const q = teju_div10(m);
  if ((uint32_t) m != 10 * ((uint32_t) q))
    return make_fields(e, m);
  return remove_trailing_zeros(q, e + 1);
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

  int32_t   const e = binary.exponent;
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

    teju_u1_t const m_a = (2 * m - 1) << r;
    teju_u1_t const a   = teju_mshift(m_a, u, l);
    teju_u1_t const m_b = (2 * m + 1) << r;
    teju_u1_t const b   = teju_mshift(m_b, u, l);
    teju_u1_t const q   = teju_div10(b);
    teju_u1_t const s   = 10 * q;

    if (s >= a) {
      if (s == b) {
        if (m % 2 == 0 || !is_tie(m_b, f))
          return remove_trailing_zeros(q, f + 1);
      }
      else if (s > a || (m % 2 == 0 && is_tie(m_a, f)))
        return remove_trailing_zeros(q, f + 1);
    }

    if ((a + b) % 2 == 1)
      return make_fields(f, (a + b) / 2 + 1);

    teju_u1_t const m_c = (2 * 2 * m) << r;
    teju_u1_t const c_2 = teju_mshift(m_c, u, l);
    teju_u1_t const c   = c_2 / 2;

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
      return make_fields(f, c);

    return make_fields(f, c + 1);
  }

  teju_u1_t const m_b = 2 * m_0 + 1;
  teju_u1_t const b   = teju_mshift(m_b << r, u, l);

  teju_u1_t const m_a = 4 * m_0 - 1;
  teju_u1_t const a   = teju_mshift(m_a << r, u, l) / 2;

  if (b > a) {

    teju_u1_t const q = teju_div10(b);
    teju_u1_t const s = 10 * q;

    if (s > a || (s == a && is_tie_uncentred(f)))
      return remove_trailing_zeros(q, f + 1);

    // m_c = 2 * 2 * m_0 = 2 * 2 * 2^{teju_mantissa_size}
    // c_2 = teju_mshift(m_c << r, upper, lower);
    uint32_t  const log2_m_c = teju_mantissa_size + 2;
    teju_u1_t const c_2      = mshift_pow2(log2_m_c + r, u, l);
    teju_u1_t const c        = c_2 / 2;

    if (c == a && !is_tie_uncentred(f))
      return make_fields(f, c + 1);

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
      return make_fields(f, c);

    return make_fields(f, c + 1);
  }

  else if (is_tie_uncentred(f))
    return remove_trailing_zeros(a, f);

  teju_u1_t const m_c = 10 * 2 * 2 * m_0;
  teju_u1_t const c_2 = teju_mshift(m_c << r, u, l);
  teju_u1_t const c   = c_2 / 2;

  if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
    return make_fields(f - 1, c);

  return make_fields(f - 1, c + 1);
}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_TEJU_H_
