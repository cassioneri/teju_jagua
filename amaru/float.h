#ifndef AMARU_AMARU_FLOAT_H_
#define AMARU_AMARU_FLOAT_H_

#include "amaru/generated/ieee32_compact.h"
#include "amaru/generated/ieee32_full.h"

#ifdef __cplusplus
extern "C" {
#endif

amaru_fields_32_t
amaru_from_float_to_fields(float value);

amaru_fields_32_t
amaru_from_float_to_decimal_compact(float value);

amaru_fields_32_t
amaru_from_float_to_decimal_full(float value);

uint32_t
amaru_from_float_to_string_compact(float value, char* str);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_FLOAT_H_
