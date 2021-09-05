#include <stdint.h>

#if !defined(AMARU_FP)
  #error "Include config<SIZE>.h before amaru.h."
#endif

typedef struct {
  bool        negative;
  int         exponent;
  AMARU_SUINT mantissa;
} AMARU_REP;

AMARU_REP AMARU_TO_DECIMAL(AMARU_FP value);

#undef AMARU_EXPONENT_SIZE
#undef AMARU_MANTISSA_SIZE
#undef AMARU_LARGE_EXPONENT
#undef AMARU_FP
#undef AMARU_SUINT
#undef AMARU_DUINT
#undef AMARU_REP
#undef AMARU_TO_DECIMAL
#undef AMARU_TABLE
