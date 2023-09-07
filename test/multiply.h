#ifndef AMARU_TEST_MULTIPLY_H_
#define AMARU_TEST_MULTIPLY_H_

#include <cstdint>

// clang and gcc support __uint128_t and hence, amaru_multiply is undefined
// for 64-bits. To force amaru_synthetic_1 tests on these platforms, we
// define amaru_multiply here.
#if defined(__clang__) || defined(__GNUC__)
  static inline
  uint64_t
  amaru_multiply(uint64_t const a, uint64_t const b, uint64_t* upper) {
      __uint128_t prod = ((__uint128_t) a) * b;
      *upper = prod >> 64;
      return prod;
  }
#endif

#endif // AMARU_TEST_MULTIPLY_H_
