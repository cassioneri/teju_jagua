#ifndef AMARU_AMARU_CONFIG_H_
#define AMARU_AMARU_CONFIG_H_

#include <stdint.h>
#if defined(_MSC_VER)
  #include <intrin.h>
#endif

//--------------------------------------------------------------------------
// Flags indicating the platform's multiplication capability.
//--------------------------------------------------------------------------

// The size in bits of the limb is amaru_size. For instance, if amaru_size
// == 32, then 1-, 2- and 4-limb integers have size 32, 64 and 128,
// respectively. Similarly, if amaru_size == 64, then 1-, 2- and 4-limb
// integers have size 64, 128 and 256 respectively. The platform is required
// to support 1-limb integers but not necessarily 2- and 4-limb integers.

// Macro amaru_multiply_type define the platform's multiplication capability
// and is set to one of the values below.

/**
 * @brief The platform provides operator * for 1-limb unsigned integers that
 * yields the lower 1-limb of the 2-limb product.
 */
#define amaru_built_in_1 0

/**
 * @brief The platform implements amaru_multiply() which takes two 1-limb
 * unsigned integers and returns the lower 1-limb of the 2-limb product. It
 * also takes a third argument of pointer type where the upper 1-limb of the
 * product is stored on exit.
 */
#define amaru_syntectic_1 1

/**
 * @brief The platform provides operator * for 2-limb unsigned integers that
 * yields the lower 2-limb of the 4-limb product.
 */
#define amaru_built_in_2  2

/**
 * @brief The platform implements amaru_multiply() which takes two 2-limb
 * unsigned integers and returns the lower 2-limb of the 4-limb product. It
 * also takes a third argument of pointer type where the upper 2-limb of the
 * product is stored on exit.
 */
#define amaru_syntectic_2 3

/**
 * @brief The platform provides operator * for 4-limb unsigned integers that
 * yields the lower 4-limb of the 8-limb product.
 */
#define amaru_built_in_4  4

//--------------------------------------------------------------------------
// Limbs
//--------------------------------------------------------------------------

// Macros amaru<X>_u1_t, where <X> = amaru_size (e.g., amaru32_u1_t for
// amaru_size = 32), is set to the type of 1-limb unsigned integers, i.e.,
// the one whose size is amaru_size. When defined, amaru<X>_u2_t and
// amaru<X>_u4_t are, respectively, set to the types of the 2- and 4-limb
// unsigned integers.

// 32 bits

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

// 64 bits

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

#endif // AMARU_AMARU_CONFIG_H_
