/**
 * @file amaru/double.h
 *
 * Amaru and helpers for \c double values.
 */

#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

#include "amaru/generated/ieee64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary64 representation of a \c double.
 *
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c double.
 *
  * @returns IEEE-754's binary64 representation of \e value.
 */
amaru64_fields_t
amaru_double_to_ieee64(double value);

/**
 * @brief Gets Amaru's binary representation of a given IEEE-754 binary64 one.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param ieee64            The given IEEE-754 binary64 representation.
 *
 * @returns Amaru's binary representation \e value.
 */
amaru64_fields_t
amaru_ieee64_to_amaru_binary(amaru64_fields_t ieee64);

/**
 * @brief Gets Amaru's decimal representation of a \c double.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c double.
 *
 * @returns Amaru's decimal representation of \e value.
 */
amaru64_fields_t
amaru_double_to_amaru_decimal(double value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
