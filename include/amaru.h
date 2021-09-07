#include <stdint.h>

typedef struct {
  bool        negative;
  int         exponent;
  AMARU_SUINT mantissa;
} AMARU_REP;

AMARU_REP AMARU_TO_DECIMAL(AMARU_FP value);
