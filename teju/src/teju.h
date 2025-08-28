// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/src/teju.h
 *
 * The implementation of Tejú Jaguá and some of its helpers.
 */

#ifndef TEJU_TEJU_SRC_TEJU_H_
#define TEJU_TEJU_SRC_TEJU_H_

#include "teju/src/common.h"
#include "teju/src/config.h"
#include "teju/src/div10.h"
#include "teju/src/mshift.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Helper functions.
//------------------------------------------------------------------------------

/**
 * @brief Checks whether mantissa m is multiple of 2^e.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @pre 0 <= e && e < teju_width.
 *
 * @returns true if m is multiple of 2^e and false, otherwise.
 */
static inline
bool
is_multiple_of_pow2(int32_t const e, teju_u1_t const m) {
  assert(0 <= e && (uint32_t) e < teju_width);
  return (m >> e) << e == m;
}

/**
 * @brief Checks whether n is a "small" multiple of 5^f.
 *
 * @param  f                The exponent f.
 * @param  n                The number n.
 *
 * @pre 0 <= f && f < sizeof(minverse) / sizeof(minverse[0]).
 *
 * @returns true if n is multiple of 5^f and false, otherwise.
 */
static inline
bool
is_multiple_of_pow5(int32_t const f, teju_u1_t const n) {
  assert(0 <= f && (uint32_t) f < sizeof(minverse) / sizeof(minverse[0]));
  return (teju_u1_t) (1u * n * minverse[f].multiplier) <= minverse[f].bound;
}

/**
 * @brief Rotates the bits of a given number 1 position to the right.
 *
 * @param  m                The given number.
 *
 * @returns The value of m after the rotation.
 */
static inline
teju_u1_t
ror(teju_u1_t const m) {
  return m << (teju_width - 1u) | m >> 1u;
}

/**
 * @brief Creates a teju_fields_t from exponent and mantissa.
 *
 * @param  e                The exponent.
 * @param  m                The mantissa.
 *
 * @returns The teju_fields_t object.
 */
static inline
teju_fields_t
make_fields(int32_t const e, teju_u1_t const m) {
  teju_fields_t const fields = {e, m};
  return fields;
}

/**
 * @brief Shortens the decimal representation of m * 10^e by removing trailing
 *        zeros from m and increasing e accordingly.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @returns The fields of the shortest close decimal representation.
 */
static inline
teju_fields_t
remove_trailing_zeros(int32_t e, teju_u1_t m) {
  // Subtracting from zero prevents msvc warning C4146.
  teju_u1_t const minv5 = 0u - ((teju_u1_t) -1) / 5u;
  teju_u1_t const bound = ((teju_u1_t) -1) / 10u + 1u;
  while (true) {
    teju_u1_t const q = ror(1u * m * minv5);
    if (q >= bound)
      return make_fields(e, m);
    ++e;
    m = q;
  }
}

//------------------------------------------------------------------------------
// Tejú Jaguá
//------------------------------------------------------------------------------

/**
 * @brief Checks whether the number m * 2^e is a "small" integer.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @returns true if m * 2^e is a "small" integer and false, otherwise.
 */
static inline
bool
is_small_integer(int32_t const e, teju_u1_t const m) {
  return 0 <= -e && (uint32_t) -e < teju_mantissa_width &&
    is_multiple_of_pow2(-e, m);
}

/**
 * @brief Finds the shortest decimal representation of x = m * 2^e when x is a
 *        "small" integer.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @returns The shortest decimal representation.
 */
static inline
teju_fields_t
to_decimal_small_integer(int32_t const e, teju_u1_t const m) {
  return remove_trailing_zeros(0, 1u * m >> -e);
}

/**
 * @brief The mantissa of uncentred floating-point numbers.
 */
static
teju_u1_t const mantissa_uncentred =
  teju_pow2(teju_u1_t, teju_mantissa_width - 1u);

/**
 * @brief Checks whether m * 2^e is a centred floating-point number.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @returns true if m * 2^e is a centred floating-point number and false,
 *          otherwise.
 */
static inline
bool
is_centred(int32_t const e, teju_u1_t const m) {
  return m != mantissa_uncentred || e == teju_exponent_min;
}

/**
 * @brief Checks whether m, for m in { m_a, m_b, c_2 }, yields a tie.
 *
 * When called to detect a tie between c * 10^f and (c + 1) * 10^f, i.e., for
 * m = c_2, the result is unspecified if is_closer_to_left(c) == true.
 *
 * @param  f                The exponent f, when m == m_a and m == m_b, or
 *                          its negation -f when m == c_2.
 * @param  m                The number m.
 *
 * @returns true if m yields a tie and false, otherwise.
 */
static inline
bool
is_tie(int32_t const f, teju_u1_t const m) {
  return 0 <= f && (uint32_t) f < sizeof(minverse) / sizeof(minverse[0]) &&
    is_multiple_of_pow5(f, m);
}

/**
 * @brief Checks whether mantissa m wins the tiebreak against its neighbour.
 *
  * Implements the ties-to-even rule, i.e., m wins the tiebreak if it's even.
  * Contrarily to other tie-breaking rules, this one doesn't depend on the
  * neighbour that m is competing against or the sign of the floating-point
  * number.
  *
 * @param  m                The mantissa m.
 *
 * @returns true if m wins the tiebreak and false, otherwise.
 */
static inline
bool
wins_tiebreak(teju_u1_t const m) {
  return m % 2u == 0;
}

/**
 * @brief Assuming m * 2^e in [c * 10^f, (c + 1) * 10^f], this function checks
 *        whether m * 2^e is closer to c * 10^f than to (c + 1) * 10^f.
 *
 * @param  c_2               The number c_2, where c = c_2 / 2.
 *
 * @returns true if m * 2^e is closer to the left and false, otherwise.
 */
static inline
bool
is_closer_to_left(teju_u1_t const c_2) {
  return c_2 % 2u == 0u;
}

/**
 * @brief Tejú Jaguá for x = m * 2^e when x is centred.
 *
 * @param  e                The exponent e.
 * @param  m                The mantissa m.
 *
 * @returns The shortest decimal representation.
 */
static inline
teju_fields_t
to_decimal_centred(int32_t const e, teju_u1_t const m) {

  int32_t           const f        = teju_log10_pow2(e);
  uint32_t          const r        = teju_log10_pow2_residual(e);
  teju_multiplier_t const M        = multipliers[f - teju_storage_index_offset];
  teju_u1_t         const m_b      = (2u * m + 1u) << r;
  teju_u1_t         const m_a      = (2u * m - 1u) << r;
  bool              const a_is_tie = is_tie(f, m_a);
  teju_u1_t         const b        = teju_mshift(m_b, M);
  teju_u1_t         const a        = teju_mshift(m_a, M) + !a_is_tie;
  teju_u1_t         const q        = teju_div10(b);
  teju_u1_t         const s        = 10u * q;

  bool const shortest =
    s == b ? !is_tie(f, m_b) || wins_tiebreak(m) :
    s == a ? !a_is_tie       || wins_tiebreak(m) :
    s >  a;
  if (shortest)
    return remove_trailing_zeros(f + 1, q);

  teju_u1_t const m_c       = 4u * m << r;
  teju_u1_t const c_2       = teju_mshift(m_c, M);
  teju_u1_t const c         = c_2 / 2u;
  bool      const pick_left = (is_tie(-f, c_2) && wins_tiebreak(c)) ||
    is_closer_to_left(c_2);

  return make_fields(f, c + !pick_left);
}

/**
 * @brief Checks whether m, for m in { m_a, m_b }, yields a tie in the uncentred
 *        case.
 *
 * @param  f                The exponent f.
 * @param  m                The number m.
 *
 * @returns true if m yields a tie and false, otherwise.
 */
static inline
bool
is_tie_uncentred(int32_t const f, teju_u1_t const m) {
  return m % 5u == 0 && is_tie(f, m);
}

/**
 * @brief Tejú Jaguá for x = m * 2^e when x is uncentred, i.e., m =
 *        mantissa_uncentred.
 *
 * @param  e                The exponent e.
 *
 * @returns The shortest decimal representation.
 */
static inline
teju_fields_t
to_decimal_uncentred(int32_t const e) {

  teju_u1_t         const m   = mantissa_uncentred;
  int32_t           const f   = teju_log10_pow2(e);
  uint32_t          const r   = teju_log10_pow2_residual(e);
  teju_multiplier_t const M   = multipliers[f - teju_storage_index_offset];
  teju_u1_t         const m_a = (4u * m - 1u) << r;
  teju_u1_t         const m_b = (2u * m + 1u) << r;
  teju_u1_t         const b   = teju_mshift(m_b, M);
  teju_u1_t         const a   = teju_mshift(m_a, M) / 2u;
  teju_u1_t         const q   = teju_div10(b);
  teju_u1_t         const s   = 10u * q;

  if (teju_calculation_sorted || a < b) {

    bool const shortest =
      s == a ?  is_tie_uncentred(f, m_a) && wins_tiebreak(m) :
      s == b ? !is_tie_uncentred(f, m_b) || wins_tiebreak(m) :
      s >  a;
    if (shortest)
      return remove_trailing_zeros(f + 1, q);

    // m_c = 4 * m * 2^r = 2^{teju_mantissa_width + r + 1}
    // c_2 = teju_mshift(m_c, upper, lower);
    uint32_t  const log2_m_c = teju_mantissa_width + r + 1u;
    teju_u1_t const c_2      = mshift_pow2(log2_m_c, M);
    teju_u1_t const c        = c_2 / 2u;

    if (c == a && !is_tie_uncentred(f, m_a))
      return make_fields(f, c + 1u);

    bool const pick_left = (is_tie(-f, c_2) && wins_tiebreak(c)) ||
      is_closer_to_left(c_2);

    return make_fields(f, c + !pick_left);
  }

  if (is_tie_uncentred(f, m_a))
    return remove_trailing_zeros(f, a);

  teju_u1_t const m_c       = 40u * m << r;
  teju_u1_t const c_2       = teju_mshift(m_c, M);
  teju_u1_t const c         = c_2 / 2u;
  bool      const pick_left = (is_tie(-f, c_2) && wins_tiebreak(c)) ||
    is_closer_to_left(c_2);

  return make_fields(f - 1, c + !pick_left);
}

/**
 * @brief Finds the shortest decimal representation of a binary floating-point
 *        number.
 *
 * @param  binary           The fields of the binary representation.
 *
 * @returns The shortest decimal representation.
 */
teju_fields_t inline
teju_function(teju_fields_t const binary) {

  int32_t   const e = binary.exponent;
  teju_u1_t const m = binary.mantissa;

  if (is_small_integer(e, m))
    return to_decimal_small_integer(e, m);

  if (is_centred(e, m))
    return to_decimal_centred(e, m);

  return to_decimal_uncentred(e);
}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_SRC_TEJU_H_
