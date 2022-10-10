// This file was auto-generated. DO NOT EDIT IT.

#ifndef AMARU_AMARU_GENERATED_IEEE32_FULL_H_
#define AMARU_AMARU_GENERATED_IEEE32_FULL_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AMARU_IS_IEEE32_T_DEFINED
#define AMARU_IS_IEEE32_T_DEFINED

typedef struct {
  bool is_negative;
  int32_t exponent;
  uint32_t mantissa;
} ieee32_t;

#endif // AMARU_IS_IEEE32_T_DEFINED

ieee32_t amaru_binary_to_decimal_ieee32_full(bool is_negative, int32_t exponent, uint32_t mantissa);

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_GENERATED_IEEE32_FULL_H_
