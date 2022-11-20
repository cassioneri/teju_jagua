#ifndef AMARU_AMARU_TYPES_H_
#define AMARU_AMARU_TYPES_H_

#include <stdint.h>
#if defined(_MSC_VER)
  #include <intrin.h>
#endif

//--------------------------------------------------------------------------
// Flags indicating the platform multiplication capabilities.
//--------------------------------------------------------------------------

// Macro amaru32_u1_t is set to the type of 1-limb operands, that is, the
// unsigned integer type whose size matches amaru_size. When defined,
// amaru32_u2_t and amaru32_u4_t are, respectively, set to type of the 2-
// and 4-limb operands, that is, the unsigned integer types whose sizes are
// 2x and 4x times amaru_size.

// The following macros define the possible values of amaru_multiply_type
// to be defined later.

/**
 * \brief The platform provides operator * which takes two 1-limb operands
 * and yields the lower 1-limb of the 2-limb product.
 */
#define amaru_built_in_1 0

/**
 * \brief The platform implements amaru_multiply() which takes two 1-limb
 * operands and returns the lower 1-limb of the 2-limb product. It also
 * takes a third argument of type amaru32_u1_t* whose pointed value is set
 * to the upper 1-limb of the product.
 */
#define amaru_syntectic_1 1

/**
 * \brief The platform provides operator * which takes two 2-limb operands
 * and yields the lower 2-limb of the 4-limb product.
 */
#define amaru_built_in_2  2

/**
 * \brief The platform provides amaru_multiply() which takes two 2-limb
 * operands and returns the lower 2-limb of the 4-limb product. It also
 * takes a third argument of type amaru32_u2_t* whose pointed value is set
 * to the upper 2-limb of the product.
 */
#define amaru_syntectic_2 3

/**
 * \brief The platform provides operator * which takes two 4-limb operands
 * and yields the lower 4-limb of the 8-limb product.
 */
#define amaru_built_in_4  4

//--------------------------------------------------------------------------
// amaru_multiply
//--------------------------------------------------------------------------

// We assume the platform provides at least uint64_t.

static inline
uint64_t
amaru_multiply(uint64_t const a, uint64_t const b, uint64_t* upper) {
  #if defined(__clang__) || defined(__GNUC__)
    __uint128_t prod = ((__uint128_t) a) * b;
    *upper = prod >> 64;
    return prod;
  #elif defined(_MSC_VER)
    return _umul128(a, b, upper);
  #endif
}

//--------------------------------------------------------------------------
// 32 bits
//--------------------------------------------------------------------------

#define amaru32_u1_t uint32_t
#define amaru32_u2_t uint64_t

#if defined(__clang__) || defined(__GNUC__)
  #define amaru32_u4_t          __uint128_t
  #define amaru32_multiply_type amaru_built_in_4
#elif defined(_MSC_VER)
  #define amaru32_multiply_type amaru_syntectic_2
#endif

typedef struct {
  int32_t      exponent;
  amaru32_u1_t mantissa;
} amaru32_fields_t;

//--------------------------------------------------------------------------
// 64 bits
//--------------------------------------------------------------------------

#define amaru64_u1_t uint64_t

#if defined(__clang__) || defined(__GNUC__)
  #define amaru64_u2_t          __uint128_t
  #define amaru64_multiply_type amaru_built_in_2
#elif defined(_MSC_VER)
  #define amaru64_multiply_type amaru_syntectic_1
#endif

typedef struct {
  int32_t      exponent;
  amaru64_u1_t mantissa;
} amaru64_fields_t;

#endif // AMARU_AMARU_TYPES_H_
