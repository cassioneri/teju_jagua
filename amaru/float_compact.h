#ifndef AMARU_AMARU_FLOAT_H_
#define AMARU_AMARU_FLOAT_H_

#include "amaru/generated/ieee32_compact.h"

#ifdef __cplusplus
extern "C" {
#endif

ieee32_t
amaru_from_float_to_fields(float value);

ieee32_t
amaru_from_float_to_decimal_compact(float value);

uint32_t
amaru_from_float_to_string_compact(float value, char* str);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_FLOAT_H_
