#ifndef AMARU_AMAHU_AMARU_H_
#define AMARU_AMAHU_AMARU_H_

#include "amaru/common.h"
#include "amaru/div10.h"
#include "amaru/mshift.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
bool
is_multiple_of_pow2(amaru_u1_t const m, int32_t const e) {
  return 0 <= e && e <= amaru_mantissa_size && ((m >> e) << e) == m;
}

static inline
bool
is_multiple_of_pow5(amaru_u1_t const m, int32_t const f) {
  return 0 <= f && f < (int32_t) (sizeof(minverse) / sizeof(minverse[0])) &&
    m * minverse[f].multiplier <= minverse[f].bound;
}

static inline
amaru_u1_t
mshift_pow2(uint32_t const k, amaru_u1_t const u, amaru_u1_t const l) {
  int32_t const s = k - (amaru_calculation_shift - amaru_size);
  if (s <= 0)
    return u >> -s;
  return (u << s) | (l >> (amaru_size - s));
}

static inline
amaru_fields_t
make_decimal(int32_t const e, amaru_u1_t const m) {
  amaru_fields_t const decimal = { e, m };
  return decimal;
}

static inline
amaru_u1_t
rotr(amaru_u1_t const n, unsigned s) {
  return (n >> s) | (n << (amaru_size - s));
}

static inline
amaru_fields_t
remove_trailing_zeros(int32_t e, amaru_u1_t m) {

  amaru_u1_t const minv5 = minverse[1].multiplier;
  amaru_u1_t const inv10 = minverse[1].bound / 2;

  while (true) {
    amaru_u1_t const n = rotr(minv5 * m, 1);
    if (n > inv10)
      break;
    ++e;
    m = n;
  }

  return make_decimal(e, m);
}

amaru_fields_t
amaru_function(int32_t const e, amaru_u1_t const m) {

  if (amaru_optimisation_integer && is_multiple_of_pow2(m, -e))
    return remove_trailing_zeros(0, m >> -e);

  amaru_u1_t const m_min = amaru_pow2(amaru_u1_t, amaru_mantissa_size);

  int32_t  const f  = log10_pow2(e);
  uint32_t const de = amaru_storage_base == 10 ? log10_pow2_remainder(e) : 0;
  uint32_t const i  = (amaru_storage_base == 10 ? f : e) -
    amaru_storage_index_offset;

  amaru_u1_t const u = multipliers[i].upper;
  amaru_u1_t const l = multipliers[i].lower;

  if (m != m_min || e == amaru_exponent_minimum) {

    amaru_u1_t const m_b = (2 * m + 1) << de;
    amaru_u1_t const b   = mshift(m_b, u, l);

    amaru_u1_t const m_a = (2 * m - 1) << de;
    amaru_u1_t const a   = mshift(m_a, u, l);

    amaru_u1_t const q   = div10(b);
    amaru_u1_t const s   = 10 * q;

    if (s >= a) {

      if (s == b) {
        if (m % 2 == 0 || !is_multiple_of_pow5(m_b, f))
          return remove_trailing_zeros(f + 1, q);
      }

      else if (s > a)
        return remove_trailing_zeros(f + 1, q);

      else if (m % 2 == 0 && is_multiple_of_pow5(m_a, f)) // s == a
        return remove_trailing_zeros(f + 1, q);
    }

    if (amaru_optimisation_mid_point && (a + b) % 2 == 1)
      return make_decimal(f, (a + b) / 2 + 1);

    amaru_u1_t const m_c = 2 * 2 * m;
    amaru_u1_t const c_2 = mshift(m_c << de, u, l);
    amaru_u1_t const c   = c_2 / 2;

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_multiple_of_pow5(c_2, -f)))
      return make_decimal(f, c);

    return make_decimal(f, c + 1);
  }

  amaru_u1_t const m_b = 2 * m_min + 1;
  amaru_u1_t const b   = mshift(m_b << de, u, l);

  amaru_u1_t const m_a = 4 * m_min - 1;
  amaru_u1_t const a   = mshift(m_a << de, u, l) / 2;

  if (b > a) {

    amaru_u1_t const q = div10(b);
    amaru_u1_t const s = 10 * q;

    if (s > a || (s == a && is_multiple_of_pow5(m_a, f)))
      return remove_trailing_zeros(f + 1, q);

    // m_c = 2 * 2 * m_min
    // c_2 = mshift(m_c << de, upper, lower);
    amaru_u1_t const log2_m_c = amaru_mantissa_size + 2;
    amaru_u1_t const c_2      = mshift_pow2(log2_m_c + de, u, l);
    amaru_u1_t const c        = c_2 / 2;

    if (c == a && !is_multiple_of_pow5(m_a, f))
      return make_decimal(f, c + 1);

    if (c_2 % 2 == 0 || (c % 2 == 0 && is_multiple_of_pow5(c_2, -f)))
      return make_decimal(f, c);

    return make_decimal(f, c + 1);
  }

  else if (b == a) {
    if (is_multiple_of_pow5(m_a, f)) {
      amaru_u1_t const q = div10(a);
      return 10 * q == a ? remove_trailing_zeros(f + 1, q) :
        make_decimal(f, a);
    }
  }

  amaru_u1_t const m_c = 10 * 2 * 2 * m_min;
  amaru_u1_t const c_2 = mshift(m_c << de, u, l);
  amaru_u1_t const c   = c_2 / 2;

  if (c_2 % 2 == 1 && (c % 2 == 1 || !is_multiple_of_pow5(c_2, -f)))
    return make_decimal(f - 1, c + 1);

  return make_decimal(f - 1, c);
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_AMARU_H_
