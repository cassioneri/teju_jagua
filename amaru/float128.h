#ifndef AMARU_AMARU_FLOAT128_H_
#define AMARU_AMARU_FLOAT128_H_

/**
 * @file amaru/float128.h
 *
 * Amaru and helpers for \c float128_t values.
 */

#if defined(__clang__) || defined(__GNUC__)

#include "amaru/generated/ieee128_compact.h"
#include "amaru/generated/ieee128_full.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary128 representation of a \c float128_t.
 *
 * See https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float128_t.
 *
  * @returns IEEE-754's binary128 representation of \e value.
 */
amaru128_fields_t
amaru_float128_to_ieee128(float128_t value);

/**
 * @brief Gets Amaru's binary representation of a given IEEE-754 binary128 one.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param ieee128           The given IEEE-754 binary128 representation.
 *
 * @returns Amaru's binary representation \e value.
 */
amaru128_fields_t
amaru_ieee128_to_amaru_binary(amaru128_fields_t ieee128);

/**
 * @brief Gets Amaru's decimal representation of a \c float128_t.
 *
 * This function uses algorithm based on the compact table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float128_t.
 *
 * @returns Amaru's decimal representation of \e value.
 */
amaru128_fields_t
amaru_float128_to_amaru_decimal_compact(float128_t value);

/**
 * @brief Gets Amaru's decimal representation of a \c float128_t.
 *
 * This function uses algorithm based on the full table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float128_t.
 *
 * @returns Amaru's decimal representation of \e value.
 */
amaru128_fields_t
amaru_float128_to_amaru_decimal_full(float128_t value);

#ifdef __cplusplus
}
#endif

#endif // defined(__clang__) || defined(__GNUC__)
#endif // AMARU_AMARU_FLOAT128_H_
