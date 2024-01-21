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

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks whether mantissa \f$m\f$ is multiple of \f$2^e\f$.
 *
 * @param m                 The mantissa \e m.
 * @param e                 The exponent \e e.
 */
static inline
bool
is_multiple_of_pow2(amaru_u1_t const m, int32_t const e) {
  return 0 <= e && e <= amaru_mantissa_size && ((m >> e) << e) == m;
}

/**
 * @brief Checks whether mantissa \f$m\f$ is multiple of \f$5^f\f$.
 *
 * @pre minverse[f] is well defined.
 *
 * @param m                 The mantissa \e m.
 * @param f                 The exponent \e f.
 */
static inline
bool
is_multiple_of_pow5(amaru_u1_t const m, int32_t const f) {
  return m * minverse[f].multiplier <= minverse[f].bound;
}

/**
 * @brief Checks whether m, for m in {m_a, m_b, c_2}, yields a tie.
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
 * @param f                 The exponent \e f.
 */
static inline
bool
is_tie_uncentred(int32_t const f) {
  return (f == 0 && !amaru_optimisation_integer) ||
    (f > 0 && amaru_mantissa_size % 4 == 2);
}

/**
 * @brief Returns the quotient \f$q = ((u\cdot 2^N + l)\cdot 2^k) / 2^s\f$,
 * where  \f$N\f$ is the value of \c aramu_size and \f$s\f$ is the value of
 * \c amaru_calculation_shift.
 *
 * @param k                 The exponent \e k.
 * @param u                 The upper part of the multiplicand \e u.
 * @param l                 The lower part of the multiplicand \e l.
 *
 * @returns The value of \f$q\f$.
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
 * @brief Creates \c amaru_fields_t from exponent and mantissa.
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
 * @pre <tt>s <= amaru_size</tt>.
 * 
 * @param n                 The given number.
 * @param s                 The given number of bits.
 */
static inline
amaru_u1_t
rotr(amaru_u1_t const n, unsigned s) {
  return (n >> s) | (n << (amaru_size - s));
}

/**
 * @brief Shortens the decimal representation \f$m\cdot 10^e\f$ by removing
 * trailing zeros of \e m and increasing \e e.
 *
 * @param e                 The exponent \e e.
 * @param m                 The mantissa \e m.
 *
 * @returns The fields of the shortest close decimal representation.
 */
static inline
amaru_fields_t
remove_trailing_zeros(int32_t e, amaru_u1_t m) {
  amaru_u1_t const minv5 = amaru_minverse5;
  amaru_u1_t const inv10 = ((amaru_u1_t) -1) / 10;
  amaru_u1_t       n     = rotr(minv5 * m, 1);
  while (n <= inv10) {
    ++e;
    m = n;
    n = rotr(minv5 * m, 1);
  }
  return make_fields(e, m);
}

/**
 * @brief Amaru itself.
 *
 * Finds the shortest decimal representation of f$m\cdot 2^e\f$.
 *
 * @param e                 The exponent \e e.
 * @param m                 The mantissa \e m.
 *
 * @returns The fields of the shortest decimal representation.
 */
amaru_fields_t
amaru_function(amaru_fields_t binary) {

  int32_t    const e = binary.exponent;
  amaru_u1_t const m = binary.mantissa;

  if (amaru_optimisation_integer && is_multiple_of_pow2(m, -e))
    return remove_trailing_zeros(0, m >> -e);

  amaru_u1_t const m_0  = amaru_pow2(amaru_u1_t, amaru_mantissa_size);
  int32_t    const f    = amaru_log10_pow2(e);
  uint32_t   const r    = amaru_full ? 0 : amaru_log10_pow2_residual(e);
  uint32_t   const i    = (amaru_full ? e : f) - amaru_storage_index_offset;
  amaru_u1_t const u    = multipliers[i].upper;
  amaru_u1_t const l    = multipliers[i].lower;

  if (m != m_0 || e == amaru_exponent_minimum) {

    amaru_u1_t const m_b = (2 * m + 1) << r;
    amaru_u1_t const b   = amaru_mshift(m_b, u, l);
    amaru_u1_t const m_a = (2 * m - 1) << r;
    amaru_u1_t const a   = amaru_mshift(m_a, u, l);
    amaru_u1_t const q   = amaru_div10(b);
    amaru_u1_t const s   = 10 * q;

    if (s >= a) {

      if (s == b) {
        if (m % 2 == 0 || !is_tie(m_b, f))
          return remove_trailing_zeros(f + 1, q);
      }

      else if (s > a || m % 2 == 0 && is_tie(m_a, f))
        return remove_trailing_zeros(f + 1, q);
    }

    if (amaru_optimisation_mid_point && (a + b) % 2 == 1)
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
    amaru_u1_t const log2_m_c = amaru_mantissa_size + 2;
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
