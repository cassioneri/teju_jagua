#pragma once

#include <stdint.h>

typedef struct {
  int32_t  E0;
  uint32_t L;
  uint32_t P;
} amaru_spec;

extern amaru_spec binary16;
extern amaru_spec binary32;
extern amaru_spec binary64;
extern amaru_spec binary128;
extern amaru_spec binary256;

inline static
uint64_t pow2(uint32_t exponent) {
  return ((uint64_t) 1) << exponent;
}

inline static
int32_t log10_pow2(int32_t exponent) {
  return exponent >= 0 ?
    (int32_t) (((uint64_t) 1292913986) * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}
