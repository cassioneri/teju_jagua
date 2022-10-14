#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

#include "amaru/generated/ieee64_compact.h"
#include "amaru/generated/ieee64_full.h"

#ifdef __cplusplus
extern "C" {
#endif

amaru_fields_64_t
amaru_from_double_to_fields(double value);

amaru_fields_64_t
amaru_from_double_to_decimal_compact(double value);

amaru_fields_64_t
amaru_from_double_to_decimal_full(double value);

uint32_t
amaru_from_double_to_string_compact(double value, char* str);

uint32_t
amaru_from_double_to_string_full(double value, char* str);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
