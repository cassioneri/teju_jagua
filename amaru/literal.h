/**
 * @file amaru/literal.h
 *
 * Workaround for lack of support for large literals.
 *
 * Some platforms might support \c uint128_t and, still, lack support for
 * literals of this type. This file provides two macros to workaround this
 * issue.
 */

#ifndef AMARU_AMARU_LITERAL_H_
#define AMARU_AMARU_LITERAL_H_

/**
 * @brief Constructs a number from its two halves.
 *
 * Let \f$N\f$ be the value of <tt>amaru_size / 2</tt>. Given \f$a_1\f$,
 * \f$a_0\f\in[0, 2^N[\f$. This macro expands to \f$a_1\cdot 2^N + a_0\f$.
 *
 * For instance, if </tt>amaru_size == 128</tt>, then:
 * \code{.cpp}
 *    amaru_literal2(0x0123456789abcdef, 0xfedcba9876543210) ==
 *         0x0123456789abcdeffedcba9876543210.
 * \endcode
 * (Except that the 128-bits literal on the right side might be ill formed.)
 *
 * @param a1                The value of \f$a_1\f$.
 * @param a0                The value of \f$a_0\f$.
 *
 * @returns An expression whose value is the constructed number.
 */
#define amaru_literal2(a1, a0) (                  \
    ((amaru_u1_t) (a1) << (1 * amaru_size / 2)) | \
    ((amaru_u1_t) (a0) << (0 * amaru_size / 2))   \
  )

/**
 * \brief Constructs a number from its four quarters.
 *
 * Let \f$N\f$ be the value of <tt>amaru_size / 2</tt>. Given \f$a_3\f$,
 * \f$a_2\f$, \f$a_1\f$, \f$a_0\f\in[0, 2^N[\f$. This macro expands to
 * \f$a_3\cdot 2^{6N} + a_2\cdot 2^{4N} + a_1\cdot 2^N + a_0\f$.
 *
 * For instance, if <tt>amaru_size == 128</tt>, then:
 * \code{.cpp}
 *     amaru_literal4(0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210) =
 *         0x0123456789abcdeffedcba9876543210.
 * \endcode
 * (Except that the 128-bits literal on the right side might be ill formed.)
 *
 * @param a3                The value of \f$a_3\f$.
 * @param a2                The value of \f$a_2\f$.
 * @param a1                The value of \f$a_1\f$.
 * @param a0                The value of \f$a_0\f$.
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
