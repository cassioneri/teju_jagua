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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks whether mantissa m is multiple of 2^e.
 *
 * @param  m                The mantissa m.
 * @param  e                The exponent e.
 *
 * @pre 0 <= e && e < teju_mantissa_size.
 *
 * @returns true if m is multiple of 2^e and false, otherwise.
 */
static inline
bool
is_multiple_of_pow2(teju_u1_t const m, int32_t const e) {
  assert(0 <= e && e < teju_mantissa_size);
  return (m & ~((teju_u1_t) -1 << e)) == 0;
}

/**
 * @brief Checks whether the number m * 2^e is a "small" integer.
 *
 * @param  m                The mantissa m.
 * @param  e                The exponent e.
 *
 * @returns true if m * 2^e is a "small" integer and false, otherwise.
 */
static inline
bool
is_small_integer(teju_u1_t const m, int32_t const e) {
  return 0 <= -e && -e < teju_mantissa_size && is_multiple_of_pow2(m, -e);
}

/**
 * @brief Checks whether n is multiple of 5^f.
 *
 * @param  n                The number n.
 * @param  f                The exponent f.
 *
 * @pre f < sizeof(minverse) / sizeof(minverse[0])
 *
 * @returns true if n is multiple of 5^f and false, otherwise.
 */
static inline
bool
is_multiple_of_pow5(teju_u1_t const n, int32_t const f) {
  assert(f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])));
  return ((teju_u1_t) (1u * n * minverse[f].multiplier)) <= minverse[f].bound;
}

/**
 * @brief Checks whether m, for m in { m_a, m_b, c_2 }, yields a tie.
 *
 * @param  m                The number m.
 * @param  f                The exponent f (for m == m_a and m == m_b) or
 *                          its negation -f for (m == c_2).
 *
 * @returns true if m yields a tie and false, otherwise.
 */
static inline
bool
is_tie(teju_u1_t const m, int32_t const f) {
  return 0 <= f && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    is_multiple_of_pow5(m, f);
}

/**
 * @brief Checks whether m_a for the uncentred value yields a tie.
 *
 * @param  m                The number m_a.
 * @param  f                The exponent f.
 *
 * @returns true if m_a yields a tie and false, otherwise.
 */
static inline
bool
is_tie_uncentred(teju_u1_t const m_a, int32_t const f) {
  return 0 <= f && m_a % 5u == 0u && is_multiple_of_pow5(m_a, f);
}

/**
 * @brief Creates a teju_fields_t from exponent and mantissa.
 *
 * @param  m                The mantissa.
 * @param  e                The exponent.
 *
 * @returns The teju_fields_t object.
 */
static inline
teju_fields_t
make_fields(teju_u1_t const m, int32_t const e) {
  teju_fields_t const fields = { m, e };
  return fields;
}

/**
 * @brief Rotates the bits of a given number 1 position to the right.
 *
 * @param  m                The given number.
 *
 * @returns The value of m after the rotation.
 */
static inline
teju_u1_t ror(teju_u1_t m) {
  return m << (teju_size - 1u) | m >> 1u;
}

/**
 * @brief Shortens the decimal representation of m * 10^e by removing trailing
 *        zeros from m and increasing e accordingly.
 *
 * @param  m                The mantissa m.
 * @param  e                The exponent e.
 *
 * @returns The fields of the shortest close decimal representation.
 */
static inline
teju_fields_t
remove_trailing_zeros(teju_u1_t m, int32_t e) {
  teju_u1_t const minv5 = -(((teju_u1_t) -1) / 5u);
  teju_u1_t const bound = ((teju_u1_t) -1) / 10u + 1u;
  while (true) {
    teju_u1_t const q = ror((teju_u1_t) (1u * m * minv5));
    if (q >= bound)
      return make_fields(m, e);
    ++e;
    m = q;
  }
}

/**
 * @brief Finds the shortest decimal representation of m * 2^e.
 *
 * This is Teju Jagua itself.
 *
 * @param  binary           The fields of the binary representation.
 *
 * @returns The fields of the shortest decimal representation.
 */
teju_fields_t
teju_function(teju_fields_t const binary) {

  int32_t   const e = binary.exponent;
  teju_u1_t const m = binary.mantissa;

  if (is_small_integer(m, e))
    return remove_trailing_zeros(m >> -e, 0);

  int32_t   const f   = teju_log10_pow2(e);
  uint32_t  const r   = teju_log10_pow2_residual(e);
  uint32_t  const i   = f - teju_storage_index_offset;
  teju_u1_t const u   = multipliers[i].upper;
  teju_u1_t const l   = multipliers[i].lower;

  teju_u1_t const m_0 = teju_pow2(teju_u1_t, teju_mantissa_size - 1u);
  if (m != m_0 || e == teju_exponent_min) {

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

  teju_u1_t const m_a = (4u * m_0 - 1u) << r;
  teju_u1_t const a   = teju_mshift(m_a, u, l) / 2u;
  teju_u1_t const m_b = (2u * m_0 + 1u) << r;
  teju_u1_t const b   = teju_mshift(m_b, u, l);

  if (teju_calculation_sorted || a < b) {

    teju_u1_t const q = teju_div10(b);
    teju_u1_t const s = 10u * q;

    if (s > a || (s == a && is_tie_uncentred(m_a, f)))
      return remove_trailing_zeros(q, f + 1);

    // m_c = 4 * m_0 * 2^r = 2^{teju_mantissa_size + r + 1}
    // c_2 = teju_mshift(m_c, upper, lower);
    uint32_t  const log2_m_c = teju_mantissa_size + r + 1u;
    teju_u1_t const c_2      = mshift_pow2(log2_m_c, u, l);
    teju_u1_t const c        = c_2 / 2u;

    if (c == a && !is_tie_uncentred(m_a, f))
      return make_fields(c + 1u, f);

    if (c_2 % 2u == 0 || (c % 2u == 0 && is_tie(c_2, -f)))
      return make_fields(c, f);

    return make_fields(c + 1u, f);
  }

  else if (is_tie_uncentred(m_a, f))
    return remove_trailing_zeros(a, f);

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
