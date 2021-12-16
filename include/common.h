#pragma once

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
#define AMARU_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
#define AMARU_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#endif

#define AMARU_POW2(type, n) (((type) 1) << n)

#define AMARU_LOWER_BITS(n, n_bits) \
  ((~(n^n) >> (CHAR_BIT * sizeof(n) - (n_bits))) & n)

static inline
int32_t log10_pow2(uint64_t n) {
  return (int32_t)(1292913987*n >> 32);
}

#ifdef __cplusplus
}
#endif
