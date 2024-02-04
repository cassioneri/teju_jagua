/**
 * @file amaru/double.h
 *
 * Amaru and helpers for double values.
 */

#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

#if defined(AMARU_HAS_UINT128)
  #include "amaru/generated/ieee64_with_uint128.h"
#else
  #include "amaru/generated/ieee64_no_uint128.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary64 representation of a double.
 *
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 *
 * @pre value > 0.
 *
 * @param value             The given double.
 *
  * @returns IEEE-754's binary64 representation of value.
 */
amaru64_fields_t
amaru_double_to_ieee64(double value);

/**
 * @brief Gets Amaru's binary representation of a given IEEE-754 binary64 one.
 *
 * @pre value > 0.
 *
 * @param ieee64            The given IEEE-754 binary64 representation.
 *
 * @returns Amaru's binary representation value.
 */
amaru64_fields_t
amaru_ieee64_to_amaru_binary(amaru64_fields_t ieee64);

/**
 * @brief Gets Amaru's decimal representation of a double.
 *
 * @pre value > 0.
 *
 * @param value             The given double.
 *
 * @returns Amaru's decimal representation of value.
 */
amaru64_fields_t
amaru_double_to_amaru_decimal(double value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
