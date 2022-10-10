#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

#include "amaru/generated/ieee64_compact.h"

#ifdef __cplusplus
extern "C" {
#endif

ieee64_t
amaru_from_double_to_fields(double value);

ieee64_t
amaru_from_double_to_decimal_compact(double value);

uint32_t
amaru_from_double_to_string_compact(double value, char* str);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
