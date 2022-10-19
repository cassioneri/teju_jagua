#ifndef AMARU_AMARU_TYPES_H_
#define AMARU_AMARU_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
#define AMARU_HAS_UINT_128_T
#endif

typedef uint32_t amaru32_limb1_t;
typedef uint64_t amaru32_limb2_t;
#ifdef AMARU_HAS_UINT_128_T
#define AMARU32_MAX_LIMBS 4
typedef __uint128_t amaru32_limb4_t;
#else
#define AMARU32_MAX_LIMBS 2
#endif

typedef struct {
  bool            is_negative;
  int32_t         exponent;
  amaru32_limb1_t mantissa;
} amaru32_fields_t;

typedef uint64_t amaru64_limb1_t ;
#ifdef AMARU_HAS_UINT_128_T
#define AMARU64_MAX_LIMBS 2
typedef __uint128_t amaru64_limb2_t;
#else
#define AMARU64_MAX_LIMBS 1
#endif

typedef struct {
  bool            is_negative;
  int32_t         exponent;
  amaru64_limb1_t mantissa;
} amaru64_fields_t;

typedef struct {
  uint32_t size;
  struct {
   int32_t minimum;
 } exponent;
  struct {
    int32_t size;
  } mantissa;
  struct {
    uint32_t limbs;
    uint32_t is_compact;
    int32_t  index_offset;
  } storage;
  struct {
    uint32_t shift;
  } calculation;
} amaru_data_t;

#endif // AMARU_AMARU_TYPES_H_
