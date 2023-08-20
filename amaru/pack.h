#ifndef AMARU_AMARU_PACK_H_
#define AMARU_AMARU_PACK_H_

/**
 * \file Workaround for lack of support for large literals.
 *
 * Some platforms might support __uint128_t and, still, lack support for
 * literals of this type. This file provides two macros to workaround this
 * issue.
 */

/**
 * \brief Reconstructs a number from its two binary halves.
 *
 * Let N = amaru_size / 2 and x = 2^N. Given a1, a0 in [0, x[ this macro
 * expands to a1 * x + a0.
 *
 * For instance, if amaru_size = 128, then we have:
 *
 *    amaru_pack2(0x0123456789abcdef, 0xfedcba9876543210) ==
 *         0x0123456789abcdeffedcba9876543210.
 */
#define amaru_pack2(a1, a0) (                     \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 2)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 2))   \
  )

/**
 * \brief Reconstructs a number from its 4 binary quarters.
 *
 * Let N = amaru_size / 4 and x = 2^N. Given a3, a2, a1, a0 in [0, x[ this macro
 * expands to a3 * x^3 + a2 * x^2 + a1 * x + a0.
 *
 * For instance, if amaru_size = 128, then we have:
 *
 *     amaru_pack(0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210) =
 *         0x0123456789abcdeffedcba9876543210.
 */
#define amaru_pack4(a3, a2, a1, a0) (             \
    ((amaru_u1_t) (a3) << (3 * amaru_size / 4)) | \
    ((amaru_u1_t) (a2) << (2 * amaru_size / 4)) | \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 4)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 4))   \
  )

#endif // AMARU_AMARU_PACK_H_
