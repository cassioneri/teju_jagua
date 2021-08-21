# pragma once

#include <stdint.h>

#ifndef AMARU_FLOAT
  #error "Include config<SIZE>.h before amaru.h."
#endif

typedef struct {
  bool         negative;
  int          exponent;
  AMARU_SINGLE mantissa;
} AMARU_REP;

AMARU_REP AMARU_TO_DECIMAL(AMARU_FLOAT value);

#undef AMARU_FLOAT

#undef AMARU_EXPONENT_SIZE
#undef AMARU_MANTISSA_SIZE
#undef AMARU_SHIFT

#undef AMARU_SINGLE
#undef AMARU_DOUBLE
#undef AMARU_REP
#undef AMARU_TO_DECIMAL
#undef AMARU_TABLE
