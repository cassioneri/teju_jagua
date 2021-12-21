#pragma once

// This file is auto-generated. DO NOT EDIT IT.

#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool    negative;
  int32_t exponent;
  uint32_t mantissa;
} ieee32_t;

ieee32_t to_amaru_dec_ieee32(ieee32_t amaru_bin);

#ifdef __cplusplus
}
#endif
