#pragma once

#include "amaru/generated/ieee64.h"

#ifdef __cplusplus
extern "C" {
#endif

ieee64_t
amaru_from_double_to_decimal(double value);

uint32_t
amaru_from_double_to_string(double value, char* str);

#ifdef __cplusplus
}
#endif
