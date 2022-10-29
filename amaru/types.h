#ifndef AMARU_AMARU_TYPES_H_
#define AMARU_AMARU_TYPES_H_

#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
  #define amaru_has_uint128_t
#endif

//--------------------------------------------------------------------------
// 32 bits
//--------------------------------------------------------------------------

typedef uint32_t amaru32_limb1_t;
typedef uint64_t amaru32_limb2_t;

#if !defined(amaru_has_uint128_t)
  #define amaru32_max_limbs 2
#else
  #define amaru32_max_limbs 4
  typedef __uint128_t amaru32_limb4_t;
#endif

typedef struct {
  int32_t         exponent;
  amaru32_limb1_t mantissa;
} amaru32_fields_t;

//--------------------------------------------------------------------------
// 64 bits
//--------------------------------------------------------------------------

typedef uint64_t amaru64_limb1_t;

#if !defined(amaru_has_uint128_t)
  #define amaru64_max_limbs 1
#else
  #define amaru64_max_limbs 2
  typedef __uint128_t amaru64_limb2_t;
#endif

typedef struct {
  int32_t         exponent;
  amaru64_limb1_t mantissa;
} amaru64_fields_t;

#endif // AMARU_AMARU_TYPES_H_
