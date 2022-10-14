#ifndef AMARU_AMARU_TYPES_H_
#define AMARU_AMARU_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
#define AMARU_HAS_UINT_128_T
#endif

typedef uint32_t amaru_32_limb1_t;
typedef uint64_t amaru_32_limb2_t;
#ifdef AMARU_HAS_UINT_128_T
#define AMARU_32_MAX_LIMBS 4
typedef __uint128_t amaru_32_limb4_t;
#else
#define AMARU_32_MAX_LIMBS 2
#endif

typedef struct {
  bool             is_negative;
  int32_t          exponent;
  amaru_32_limb1_t mantissa;
} amaru_fields_32_t;


typedef uint64_t amaru_64_limb1_t ;
#ifdef AMARU_HAS_UINT_128_T
#define AMARU_64_MAX_LIMBS 2
typedef __uint128_t amaru_64_limb2_t;
#else
#define AMARU_64_MAX_LIMBS 1
#endif

typedef struct {
  bool             is_negative;
  int32_t          exponent;
  amaru_64_limb1_t mantissa;
} amaru_fields_64_t;

#endif // AMARU_AMARU_TYPES_H_
