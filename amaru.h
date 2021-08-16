# pragma once

#include "common.h"

#ifndef AMARU_SIZE
  #error "Include config<SIZE>.h before amaru.h."
#endif

typedef struct {
  int        sign;
  int        exponent;
  AMARU_UINT mantissa;
} AMARU_REP;

void AMARU_TO_DECIMAL(AMARU_TYPE in, AMARU_REP* out);

#undef AMARU_SIZE
#undef AMARU_EXPONENT_SIZE
#undef AMARU_MANTISSA_SIZE

#undef AMARU_TYPE
#undef AMARU_UINT
#undef AMARU_UBIGINT
#undef AMARU_REP

#undef AMARU_TO_DECIMAL
