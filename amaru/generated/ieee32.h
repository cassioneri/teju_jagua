// This file was auto-generated. DO NOT EDIT IT.

#ifndef AMARU_GENERATED_IEEE32_COMPACT_H_
#define AMARU_GENERATED_IEEE32_COMPACT_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool is_negative;
  int32_t exponent;
  uint32_t mantissa;
} ieee32_t;

ieee32_t amaru_binary_to_decimal_ieee32(bool is_negative, int32_t exponent, uint32_t mantissa);

#ifdef __cplusplus
}
#endif

#endif // AMARU_GENERATED_IEEE32_COMPACT_H_
