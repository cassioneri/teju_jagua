#include <stdint.h>

#ifndef AMARU_FP
  #error "Include config<SIZE>.h before amaru.h."
#endif

typedef struct {
  bool              negative;
  int               exponent;
  AMARU_UINT_SINGLE mantissa;
} AMARU_REP;

AMARU_REP AMARU_TO_DECIMAL(AMARU_FP value);

#undef AMARU_EXPONENT_SIZE
#undef AMARU_MANTISSA_SIZE
#undef AMARU_LARGE

#undef AMARU_FP
#undef AMARU_UINT_SINGLE
#undef AMARU_UINT_DOUBLE
#undef AMARU_REP
#undef AMARU_TO_DECIMAL
#undef AMARU_TABLE
