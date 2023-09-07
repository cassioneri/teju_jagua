#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

/**
 * @file amaru/double.h
 *
 * Amaru and helpers for \c double values.
 */

#include "amaru/generated/ieee64_compact.h"
#include "amaru/generated/ieee64_full.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the IEEE-754 fields of a given \c double.
 *
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c double.
 *
 * @returns The IEEE-754 fields of \e value.
 */
amaru64_fields_t
amaru_from_double_to_fields(double value);

/**
 * @brief Gets the Amaru fields of a given \c double.
 *
 * This function uses algorithm based on the compact table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c double.
 *
 * @returns The Amaru fields of \e value.
 */
amaru64_fields_t
amaru_from_double_to_decimal_compact(double value);

/**
 * @brief Gets the Amaru fields of a given \c double.
 *
 * This function uses algorithm based on the full table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c double.
 *
 * @returns The Amaru fields of \e value.
 */
amaru64_fields_t
amaru_from_double_to_decimal_full(double value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
