/**
 * @file amaru/amaru.h
 *
 * The implementation of Amaru and some of its helpers.
 */

#ifndef AMARU_AMARU_AMARU_H_
#define AMARU_AMARU_AMARU_H_

#include "amaru/common.h"
#include "amaru/config.h"
#include "amaru/div10.h"
#include "amaru/mshift.h"

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
 * @pre 0 <= e && e <= amaru_mantissa_size.
 *
 * @param m                 The mantissa m.
 * @param e                 The exponent e.
 */
static inline
bool
is_multiple_of_pow2(amaru_u1_t const m, int32_t const e) {
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
is_small_integer(amaru_u1_t const m, int32_t const e) {
  return 0 <= -e && -e <= amaru_mantissa_size && is_multiple_of_pow2(m, -e);
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
is_multiple_of_pow5(amaru_u1_t const m, int32_t const f) {
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
is_tie(amaru_u1_t const m, int32_t const f) {
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
  return f > 0 && amaru_mantissa_size % 4 == 2;
}

/**
 * @brief Returns the quotient q = ((u * 2^N + l) * 2^k) / 2^s, where
 * N = aramu_size and s = amaru_calculation_shift.
 *
 * @param k                 The exponent k.
 * @param u                 The upper part of the multiplicand u.
 * @param l                 The lower part of the multiplicand l.
 *
 * @returns The value of q.
 */
static inline
amaru_u1_t
mshift_pow2(uint32_t const k, amaru_u1_t const u, amaru_u1_t const l) {
  int32_t const s = k - (amaru_calculation_shift - amaru_size);
  if (s <= 0)
    return u >> -s;
  return (u << s) | (l >> (amaru_size - s));
}

/**
 * @brief Creates amaru_fields_t from exponent and mantissa.
 *
 * @param e                 The exponent.
 * @param m                 The mantissa.
 */
static inline
amaru_fields_t
make_fields(int32_t const e, amaru_u1_t const m) {
  amaru_fields_t const fields = { e, m };
  return fields;
}

/**
 * @brief Rotates a given number to the right by a given number of bits.
 *
 * @pre s <= amaru_size.
 * 
 * @param n                 The given number.
 * @param s                 The given number of bits.
 */
static inline
amaru_u1_t
ror(amaru_u1_t const n) {
  #if defined(_MSC_VER)
    #if amaru_size == 32
      return _rotr(n, 1);
    #elif amaru_size == 64
      return _rotr64(n, 1);
    #else
      #error "Unsupported size for ror."
    #endif
  #else
    return (n >> 1) | (n << (amaru_size - 1));
  #endif
}

/**
 * @brief Shortens the decimal representation m\cdot 10^e\f by removing trailing
 * zeros of m and increasing e.
 *
 * @param e                 The exponent e.
 * @param m                 The mantissa m.
 *
 * @returns The fields of the shortest close decimal representation.
 */
static inline
amaru_fields_t
remove_trailing_zeros(int32_t e, amaru_u1_t m) {
  amaru_u1_t const minv5 = amaru_minverse5;
  amaru_u1_t const inv10 = ((amaru_u1_t) -1) / 10;
  amaru_u1_t       n     = ror(minv5 * m);
  while (n <= inv10) {
    ++e;
    m = n;
    n = ror(minv5 * m);
  }
  return make_fields(e, m);
}

/**
 * @brief Amaru itself.
 *
 * Finds the shortest decimal representation of m * 2^e.
 *
 * @param e                 The exponent e.
 * @param m                 The mantissa m.
 *
 * @returns The fields of the shortest decimal representation.
 */
amaru_fields_t
amaru_function(amaru_fields_t const binary) {

  int32_t    const e = binary.exponent;
  amaru_u1_t const m = binary.mantissa;

  if (is_small_integer(m, e))
    return remove_trailing_zeros(0, m >> -e);

  amaru_u1_t const m_0 = amaru_pow2(amaru_u1_t, amaru_mantissa_size);
  int32_t    const f   = amaru_log10_pow2(e);
  uint32_t   const r   = amaru_log10_pow2_residual(e);
  uint32_t   const i   = f - amaru_storage_index_offset;
  amaru_u1_t const u   = multipliers[i].upper;
  amaru_u1_t const l   = multipliers[i].lower;

  if (m != m_0 || e == amaru_exponent_minimum) {

    amaru_u1_t const m_a = (2 * m - 1) << r;
    amaru_u1_t const a   = amaru_mshift(m_a, u, l);
    amaru_u1_t const m_b = (2 * m + 1) << r;
    amaru_u1_t const b   = amaru_mshift(m_b, u, l);
    amaru_u1_t const q   = amaru_div10(b);
    amaru_u1_t const s   = 10 * q;

    if (s >= a) {
      if (s == b) {
        if (m % 2 == 0 || !is_tie(m_b, f))
          return remove_trailing_zeros(f + 1, q);
      }
      else if (s > a || (m % 2 == 0 && is_tie(m_a, f)))
        return remove_trailing_zeros(f + 1, q);
    }

    if ((a + b) % 2 == 1)
      return make_fields(f, (a + b) / 2 + 1);

    amaru_u1_t const m_c = (2 * 2 * m) << r;
    amaru_u1_t const c_2 = amaru_mshift(m_c, u, l);
    amaru_u1_t const c   = c_2 / 2;

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
      return make_fields(f, c);

    return make_fields(f, c + 1);
  }

  amaru_u1_t const m_b = 2 * m_0 + 1;
  amaru_u1_t const b   = amaru_mshift(m_b << r, u, l);

  amaru_u1_t const m_a = 4 * m_0 - 1;
  amaru_u1_t const a   = amaru_mshift(m_a << r, u, l) / 2;

  if (b > a) {

    amaru_u1_t const q = amaru_div10(b);
    amaru_u1_t const s = 10 * q;

    if (s > a || (s == a && is_tie_uncentred(f)))
      return remove_trailing_zeros(f + 1, q);

    // m_c = 2 * 2 * m_0 = 2 * 2 * 2^{amaru_mantissa_size}
    // c_2 = amaru_mshift(m_c << r, upper, lower);
    uint32_t   const log2_m_c = amaru_mantissa_size + 2;
    amaru_u1_t const c_2      = mshift_pow2(log2_m_c + r, u, l);
    amaru_u1_t const c        = c_2 / 2;

    if (c == a && !is_tie_uncentred(f))
      return make_fields(f, c + 1);

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
      return make_fields(f, c);

    return make_fields(f, c + 1);
  }

  else if (is_tie_uncentred(f))
    remove_trailing_zeros(f, a);

  amaru_u1_t const m_c = 10 * 2 * 2 * m_0;
  amaru_u1_t const c_2 = amaru_mshift(m_c << r, u, l);
  amaru_u1_t const c   = c_2 / 2;

  if (c_2 % 2 == 0 || (c % 2 == 0 && is_tie(c_2, -f)))
    return make_fields(f - 1, c);

  return make_fields(f - 1, c + 1);
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_AMARU_H_
