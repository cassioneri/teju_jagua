/*
clang++ -std=c++17 -O3 -DNDEBUG -I ~cassio/amaru/dragonbox/include -c dragonbox.cpp -o dragonbox.o
*/
#include "dragonbox.h"

#include <cstdint>

extern "C" {

typedef struct {
  bool    negative;
  int32_t exponent;
  uint32_t mantissa;
} ieee32_t;

typedef struct {
  bool    negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

ieee32_t dragonbox_float(float const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

ieee64_t dragonbox_double(double const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

}
