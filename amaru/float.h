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
 * @brief Gets IEEE-754's binary32 representation of a \c float.
 *
 * See https://en.wikipedia.org/wiki/Single-precision_floating-point_format
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns IEEE-754's binary32 representation of \e value.
 */
amaru32_fields_t
amaru_to_ieee32(float value);

/**
 * @brief Gets Amaru's binary representation of a given IEEE-754 binary32 one.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param ieee32            The given IEEE-754 binary32 representation.
 *
 * @returns Amaru's binary representation \e value.
 */
amaru32_fields_t
amaru_ieee32_to_amaru(amaru32_fields_t ieee32);

/**
 * @brief Gets Amaru's decimal representation of a \c float.
 *
 * This function uses algorithm based on the compact table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns Amaru's decimal representation of \e value.
 */
amaru32_fields_t
amaru_float_compact(float value);

/**
 * @brief Gets Amaru's decimal representation of a \c float.
 *
 * This function uses algorithm based on the full table storage.
 *
 * @pre <tt>value > 0</tt>.
 *
 * @param value             The given \c float.
 *
 * @returns Amaru's decimal representation of \e value.
 */
amaru32_fields_t
amaru_float_full(float value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_FLOAT_H_
