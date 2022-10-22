#ifndef AMARU_AMARU_DOUBLE_H_
#define AMARU_AMARU_DOUBLE_H_

#include "amaru/generated/ieee64_compact.h"
#include "amaru/generated/ieee64_full.h"

#ifdef __cplusplus
extern "C" {
#endif

amaru64_fields_t
amaru_from_double_to_fields(double value);

amaru64_fields_t
amaru_from_double_to_decimal_compact(double value);

amaru64_fields_t
amaru_from_double_to_decimal_full(double value);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DOUBLE_H_
