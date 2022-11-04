#ifndef AMARU_AMARU_TYPES_H_
#define AMARU_AMARU_TYPES_H_

#include <stdint.h>
#if defined(_MSC_VER)
  #include <intrin.h>
#endif

//--------------------------------------------------------------------------
// Flags indicating multiplication capabilities.
//--------------------------------------------------------------------------

// The following macros defined the possible values of amaru<size>_multiply
// to be defined later.

/**
 * \brief Platform provides operator * which takes 1-limb operands and
 * yields the lower 1-limb of the 2-limb product.
 */
#define amaru_built_in_1    0

/**
 * \brief Platform provides amaru_multiply which takes 1-limb operands and
 * yields the upper 1-limb of the 2-limb product.
 */
#define amaru_syntectic_1   1

/**
 * \brief Platform provides operator * which takes 2-limb operands and
 * yields the lower 2-limb of the 4-limb product.
 */
#define amaru_built_in_2    2

/**
 * \brief Platform provides amaru_multiply which takes 2-limb operands and
 * yields the upper 2-limb of the 4-limb product.
 */
#define amaru_syntectic_2   3

/**
 * \brief Platform provides operator * which takes 4-limb operands and
 * yields the lower 4-limb of the 8-limb product.
 */
#define amaru_built_in_4    4

static inline
uint64_t
amaru_multiply(uint64_t const a, uint64_t const b) {
  #if defined(__clang__) || defined(__GNUC__)
    return (((__uint128_t) a) * b) >> 64;
  #elif defined(_MSC_VER)
    uint64_t upper;
    (void) _umul128(a, b, &upper);
    return upper;
  #endif
}

//--------------------------------------------------------------------------
// 32 bits
//--------------------------------------------------------------------------

#define amaru32_limb1_t uint32_t
#define amaru32_limb2_t uint64_t

#if defined(__clang__) || defined(__GNUC__)
  #define amaru32_limb4_t       __uint128_t
  #define amaru32_multiply_type amaru_built_in_4
#elif defined(_MSC_VER)
  #define amaru32_multiply_type amaru_syntectic_2
#endif

typedef struct {
  int32_t         exponent;
  amaru32_limb1_t mantissa;
} amaru32_fields_t;

//--------------------------------------------------------------------------
// 64 bits
//--------------------------------------------------------------------------

#define amaru64_limb1_t uint64_t

#if defined(__clang__) || defined(__GNUC__)
  #define amaru64_limb2_t       __uint128_t
  #define amaru64_multiply_type amaru_built_in_2
#elif defined(_MSC_VER)
  #define amaru64_multiply_type amaru_syntectic_1
#endif

typedef struct {
  int32_t         exponent;
  amaru64_limb1_t mantissa;
} amaru64_fields_t;

#endif // AMARU_AMARU_TYPES_H_
