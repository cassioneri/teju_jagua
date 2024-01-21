/**
 * @file amaru/literal.h
 *
 * Workaround for lack of support for large literals.
 *
 * Some platforms might support uint128_t and, still, lack support for literals
 * of this type. This file provides two macros to workaround this issue.
 */

#ifndef AMARU_AMARU_LITERAL_H_
#define AMARU_AMARU_LITERAL_H_

/**
 * @brief Constructs a number from its two halves.
 *
 * Let N = amaru_size / 2. Given a_1, a_0 in [0, 2^N[, this macro expands to
 * a_1 * 2^N + a_0.
 *
 * For instance, if amaru_size == 128, then:
 * \code{.cpp}
 *    amaru_literal2(0x0123456789abcdef, 0xfedcba9876543210) ==
 *         0x0123456789abcdeffedcba9876543210.
 * \endcode
 * (Except that the 128-bits literal on the right side might be ill formed.)
 *
 * @param a1                The 1st (upper) half a_1.
 * @param a0                The 0th (lower) half a_0.
 *
 * @returns An expression whose value is the constructed number.
 */
#define amaru_literal2(a1, a0) (                  \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 2)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 2))   \
  )

/**
 * @brief Constructs a number from its four quarters.
 *
 * Let N = amaru_size / 4. Given a_3, a_2, a_1, a_0 in [0, 2^N[, this macro
 *  expands to a_3 * 2^(6 * N) + a_2 * 2^(4 * N) + a_1 2^N + a_0.
 *
 * For instance, if amaru_size == 128, then:
 * \code{.cpp}
 *     amaru_literal4(0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210) =
 *         0x0123456789abcdeffedcba9876543210.
 * \endcode
 * (Except that the 128-bits literal on the right side might be ill formed.)
 *
 * @param a3                The 3rd (highest) quarter a_3.
 * @param a2                The 2nd quarter a_2.
 * @param a1                The 1st quarter a_1.
 * @param a0                The 0th (lowest) quarter a_0.
 *
 * @returns An expression whose value is the constructed number.
 */
#define amaru_literal4(a3, a2, a1, a0) (          \
    ((amaru_u1_t) (a3) << (3 * amaru_size / 4)) | \
    ((amaru_u1_t) (a2) << (2 * amaru_size / 4)) | \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 4)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 4))   \
  )

#endif // AMARU_AMARU_LITERAL_H_
