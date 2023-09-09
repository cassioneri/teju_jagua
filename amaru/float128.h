#ifndef AMARU_AMARU_FLOAT128_H_
#define AMARU_AMARU_FLOAT128_H_

/**
 * @file amaru/float128.h
 *
 * Amaru and helpers for \c __float128 values.
 */

#if defined(__clang__) || defined(__GNUC__)

#include "amaru/generated/ieee128_compact.h"
#include "amaru/generated/ieee128_full.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the IEEE-754 fields of a given \c __float128.
 *
 * See https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c __float128.
 *
 * @returns The IEEE-754 fields of \e value.
 */
amaru128_fields_t
amaru_from_float128_to_fields(__float128 value);

/**
 * @brief Gets the Amaru fields of a given \c __float128.
 *
 * This function uses algorithm based on the compact table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c __float128.
 *
 * @returns The Amaru fields of \e value.
 */
amaru128_fields_t
amaru_from_float128_to_decimal_compact(__float128 value);

/**
 * @brief Gets the Amaru fields of a given \c __float128.
 *
 * This function uses algorithm based on the full table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c __float128.
 *
 * @returns The Amaru fields of \e value.
 */
amaru128_fields_t
amaru_from_float128_to_decimal_full(__float128 value);

#ifdef __cplusplus
}
#endif

#endif // defined(__clang__) || defined(__GNUC__)
#endif // AMARU_AMARU_FLOAT128_H_
