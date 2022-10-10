// This file was auto-generated. DO NOT EDIT IT.

#ifndef AMARU_AMARU_GENERATED_IEEE64_FULL_H_
#define AMARU_AMARU_GENERATED_IEEE64_FULL_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AMARU_IS_IEEE64_T_DEFINED
#define AMARU_IS_IEEE64_T_DEFINED

typedef struct {
  bool is_negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

#endif // AMARU_IS_IEEE64_T_DEFINED

ieee64_t amaru_binary_to_decimal_ieee64_full(bool is_negative, int32_t exponent, uint64_t mantissa);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_GENERATED_IEEE64_FULL_H_
