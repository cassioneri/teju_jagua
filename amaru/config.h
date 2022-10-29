#ifndef AMARU_AMARU_CONFIG_H_
#define AMARU_AMARU_CONFIG_H_

#include <stdbool.h>

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
  struct {
    bool integer;
    bool mid_point;
  } optimisation;
} config_t;

#endif // AMARU_AMARU_CONFIG_H_
