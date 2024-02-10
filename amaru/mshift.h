/**
 * @file amaru/mshift.h
 *
 * Multiply-and-shift operations.
 */

#ifndef AMARU_AMARU_MSHIFT_H_
#define AMARU_AMARU_MSHIFT_H_

#if !defined(amaru_calculation_mshift) || !defined(amaru_calculation_shift) || \
  !defined(amaru_u1_t) || !defined(amaru_size)
  #error "Macros amaru_calculation_mshift, amaru_calculation_shift, amaru_size and amaru_u1_t must be defined prior to inclusion of mshift.h."
#endif

#include "amaru/common.h"
#include "amaru/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the quotient q = (r_2 * 2^(2 * N) + r_1 * 2^N) / 2^s, where
 * N = aramu_size and s = amaru_calculation_shift.
 *
 * @param r2                The value of r_2.
 * @param r1                The value of r_1$.
 *
 * @returns The quotient q.
 */
static inline
amaru_u1_t
amaru_rshift(amaru_u1_t const r2, amaru_u1_t const r1) {

  #if amaru_calculation_shift >= 2 * amaru_size

    return r2 >> (amaru_calculation_shift - 2 * amaru_size);

  #else

    return r2 << (2 * amaru_size - amaru_calculation_shift) |
      r1 >> (amaru_calculation_shift - amaru_size);

  #endif
}

/**
 * @brief Returns the quotient q = ((u * 2^N + l) * m) / 2^s, where
 * N = aramu_size and s = amaru_calculation_shift.
 *
 * @param m                 The 1st multiplicand  m.
 * @param u                 The 2nd multiplicand upper half u.
 * @param l                 The 2nd multiplicand lower half l.
 *
 * @returns The quotient q.
 */
static inline
amaru_u1_t
amaru_mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {

  // Let x := 2^N.

  #if amaru_calculation_mshift == amaru_built_in_4

    amaru_u2_t const n = (((amaru_u2_t) u) << amaru_size) | l;
    return (((amaru_u4_t) n) * m) >> amaru_calculation_shift;

  #elif amaru_calculation_mshift == amaru_synthetic_2

    // (u * x + l) * m = r2 * x^2 + r1 * x + r0,
    //                   with r2, r1, r0 in [0, x[.

    amaru_u2_t const n = (((amaru_u2_t) u) << amaru_size) | l;
    amaru_u2_t r2;
    amaru_u2_t const r1 = amaru_multiply(n, m, &r2) >> amaru_size;
    return amaru_rshift(r2, r1);

  #elif amaru_calculation_mshift == amaru_built_in_2

    // (u * x + l) * m = s1 * x + s0,
    //                       with s1 := u * m, s0 := l * m in [0, x^2[,
    //                 = s1 * x + (s01 * x + s00)
    //                       with s01 := s0 / x, s00 := s0 % x in [0, x[,
    //                 = (s1 + s01) * x + s00.

    amaru_u2_t const s0 = ((amaru_u2_t) l) * m;
    amaru_u2_t const s1 = ((amaru_u2_t) u) * m;
    return (s1 + (s0 >> amaru_size)) >>
      (amaru_calculation_shift - amaru_size);

  #elif amaru_calculation_mshift == amaru_synthetic_1

    // (u * x + l) * m = s1 * x + s0,
    //                       with s1 := u * m, s0 := l * m in [0, x^2[,
    //                 = (s11 * x + s10) * x + (s01 * x + s00),
    //                       with s11 := s1 / x, s10 := s1 % x,
    //                            s01 := s0 / x, s00 := s0 % x in [0, x[,
    //                 = s11 * x^2 +(s10 + s01) * x + s00

    amaru_u1_t s01, s11;
    (void) amaru_multiply(l, m, &s01); // s00 is discarded
    amaru_u1_t const s10 = amaru_multiply(u, m, &s11);
    amaru_u1_t const r0  = s01 + s10; // This might wraparound.
    amaru_u1_t const c   = r0 < s01;  // Carry.
    amaru_u1_t const r1  = s11 + c;
    return amaru_rshift(r1, r0);

  #elif amaru_calculation_mshift == amaru_built_in_1

    // Let y := 2^(N / 2), so that, x = y^2. Then:
    // u := (n3 * y + n2) with n3 := u / y, n2 = u % y in [0, y[,
    // l := (n1 * y + n0) with n1 := l / y, n0 = l % y in [0, y[,
    // m := (m1 * y + m0) with m1 := m / y, m0 = m % y in [0, y[.

    amaru_u1_t const y  = amaru_pow2(amaru_u1_t, amaru_size / 2);
    amaru_u1_t const n3 = u / y;
    amaru_u1_t const n2 = u % y;
    amaru_u1_t const n1 = l / y;
    amaru_u1_t const n0 = l % y;
    amaru_u1_t const m1 = m / y;
    amaru_u1_t const m0 = m % y;

    // result, carry, temporary:
    amaru_u1_t r1, r0, c, t;

    // (u * x + l) * m
    //     = ((n3 * y + n2) * y^2 + (n1 * y + n0)) * (m1 * y + m0),
    //     = (n3 * y^3 + n2 * y^2 + n1 * y + n0) * (m1 * y + m0),
    //     = (n3 * m1) * y^4 + (n3 * m0 + n2 * m1) * y^3 +
    //       (n2 * m0 + n1 * m1) * y^2 + (n1 * m0 + n0 * m1) * y +
    //       (n0 * m0)

    // order 0:
    t  = n0 * m0;
    r1  = t / y;

    // order 1:
    r1 += n0 * m1;         // This doesn't wraparound.
    t   = n1 * m0;
    r1 += t;               // This might wraparound.
    c   = r1 < t;          // Carry.
    r1 /= y;

    // order 2:
    r1 += n1 * m1 + c * y; // This doesn't wraparound.
    t   = n2 * m0;
    r1 += t;               // This might wraparound.
    c   = r1 < t;          // Carry.
    r1 /= y;

    // order 3:
    r1 += n2 * m1 + c * y; // This doesn't wraparound.
    t   = n3 * m0;
    r1 += t;               // This might wraparound.
    c   = r1 < t;          // Carry.
    r0  = r1 % y;
    r1 /= y;

    // order 4:
    r1 += n3 * m1 + c * y;

    #if amaru_calculation_shift >= 2 * amaru_size
      (void) r0;
      return r1 >> (amaru_calculation_shift - 2 * amaru_size);
    #elif amaru_calculation_shift >= 3 * amaru_size / 2
      return (r1 << (2 * amaru_size - amaru_calculation_shift)) |
        (r0 >> (amaru_calculation_shift - 3 * amaru_size / 2));
    #else
      #error "Unsupported combination of size, shift and mshift calculation."
    #endif

  #else

    #error "Invalid definition of macro amaru_calculation_mshift."

  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_MSHIFT_H_
