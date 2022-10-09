#pragma once

#include "amaru/generated/ieee32.h"

#ifdef __cplusplus
extern "C" {
#endif

ieee32_t
amaru_from_float_to_decimal(float value);

uint32_t
amaru_from_float_to_string(float value, char* str);

#ifdef __cplusplus
}
#endif
