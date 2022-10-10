// This file was auto-generated. DO NOT EDIT IT.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool is_negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

ieee64_t amaru_binary_to_decimal_ieee64(bool is_negative, int32_t exponent, uint64_t mantissa);

#ifdef __cplusplus
}
#endif
