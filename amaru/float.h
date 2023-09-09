#ifndef AMARU_AMARU_FLOAT_H_
#define AMARU_AMARU_FLOAT_H_

/**
 * @file amaru/float.h
 *
 * Amaru and helpers for \c float values.
 */

#include "amaru/generated/ieee32_compact.h"
#include "amaru/generated/ieee32_full.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the IEEE-754 fields of a given \c float.
 *
 * See https://en.wikipedia.org/wiki/Single-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns The IEEE-754 fields of \e value.
 */
amaru32_fields_t
amaru_to_ieee32_fields(float value);

/**
 * @brief Gets the Amaru fields of a given \c float.
 *
 * This function uses algorithm based on the compact table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns The Amaru fields of \e value.
 */
amaru32_fields_t
amaru_from_float_to_decimal_compact(float value);

/**
 * @brief Gets the Amaru fields of a given \c float.
 *
 * This function uses algorithm based on the full table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns The Amaru fields of \e value.
 */
amaru32_fields_t
amaru_from_float_to_decimal_full(float value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_FLOAT_H_
