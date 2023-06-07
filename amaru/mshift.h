#ifndef AMARU_AMARU_MSHIFT_H_
#define AMARU_AMARU_MSHIFT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Returns (r2 * x^2 + r1 * x) >> amaru_calculation_shift, where
 * x := 2^amaru_size.
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
 * \brief Returns (u * x + l) * m >> amaru_calculation_shift, where
 * x := 2^amaru_size.
 */
static inline
amaru_u1_t
amaru_mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {

  #if amaru_calculation_mshift == amaru_built_in_4

    amaru_u2_t const n = (((amaru_u2_t) u) << amaru_size) | l;
    return (((amaru_u4_t) n) * m) >> amaru_calculation_shift;

  #elif amaru_calculation_mshift == amaru_syntectic_2

    // (u * x + l) * m = r2 * x^2 + r1 * x + r0,
    //                     with r2, r1, r0 in [0, x[.

    amaru_u2_t
    amaru_multiply_2(amaru_u2_t const a, amaru_u2_t const b, amaru_u2_t* upper);

    amaru_u2_t const n = (((amaru_u2_t) u) << amaru_size) | l;
    amaru_u2_t r2;
    amaru_u2_t const r1 = amaru_multiply_2(n, m, &r2) >> amaru_size;
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

  #elif amaru_calculation_mshift == amaru_syntectic_1

    // (u * x + l) * m = s1 * x + s0,
    //                       with s1 := u * m, s0 := l * m in [0, x^2[,
    //                 = (s11 * x + s10) * x + (s01 * x + s00),
    //                       with s11 := s1 / x, s10 := s1 % x,
    //                            s01 := s0 / x, s00 := s0 % x in [0, x[,
    //                 = s11 * x^2 +(s10 + s01) * x + s00

    amaru_u1_t
    amaru_multiply_1(amaru_u1_t const a, amaru_u1_t const b, amaru_u1_t* upper);

    amaru_u1_t s01, s11;
    (void) amaru_multiply_1(l, m, &s01); // s00 is discarded
    amaru_u1_t const s10 = amaru_multiply_1(u, m, &s11);
    amaru_u1_t const r0  = s01 + s10; // This might overflow.
    amaru_u1_t const c   = r0 < s01;  // Carry.
    amaru_u1_t const r1  = s11 + c;
    return amaru_rshift(r1, r0);

  #elif amaru_calculation_mshift == amaru_built_in_1

    // Let y := 2^(amaru_size / 2), so that, x = y^2. Then:
    // u := (n3 * y + n2) with n3 := u / y, n2 = u % y in [0, y[,
    // l := (n1 * y + n0) with n1 := l / y, n0 = l % y in [0, y[,
    // m := (m1 * y + m0) with m1 := m / y, m0 = m % y in [0, y[.
    amaru_u1_t const y_mask = (((amaru_u1_t) 1) << (amaru_size / 2)) - 1;
    amaru_u1_t const n3 = u >> (amaru_size / 2);
    amaru_u1_t const n2 = u & y_mask;
    amaru_u1_t const n1 = l >> (amaru_size / 2);
    amaru_u1_t const n0 = l & y_mask;
    amaru_u1_t const m1 = m >> (amaru_size / 2);
    amaru_u1_t const m0 = m & y_mask;

    // result, carry, temporary:
    amaru_u1_t r, c, t;

    // (u * x + l) * m
    //     = ((n3 * y + n2) * y^2 + (n1 * y + n0)) * (m1 * y + m0),
    //     = (n3 * y^3 + n2 * y^2 + n1 * y + n0) * (m1 * y + m0),
    //     = (n3 * m1) * y^4 + (n3 * m0 + n2 * m1) * y^3 +
    //       (n2 * m0 + n1 * m1) * y^2 + (n1 * m0 + n0 * m1) * y +
    //       (n0 * m0)

    // order 0:
    t   = n0 * m0;
    r   = t >> (amaru_size / 2);

    // order 1:
    r  += n0 * m1; // This doesn't overflow.
    t   = n1 * m0;
    r  += t;       // This might overflow.
    c   = r < t;   // Carry.
    r >>= (amaru_size / 2);

    // order 2:
    r  += n1 * m1 + (c << (amaru_size / 2)); // This doesn't overflow.
    t   = n2 * m0;
    r  += t;                                 // This might overflow.
    c   = r < t;                             // Carry.
    r >>= (amaru_size / 2);

    // order 3:
    r  += n2 * m1 + (c << (amaru_size / 2)); // This doesn't overflow.
    t   = n3 * m0;
    r  += t;                                 // This might overflow.
    c   = r < t;                             // Carry.
    r >>= (amaru_size / 2);

    // order 4:
    r  += n3 * m1 + (c << (amaru_size / 2));

    #if amaru_calculation_shift >= 2 * amaru_size
      return r >> (amaru_calculation_shift - 2 * amaru_size);
    #else
      #error "Unsupported combination of size, shift and calculation."
    #endif

  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_MSHIFT_H_
