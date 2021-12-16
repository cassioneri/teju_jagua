// g++ -O3 -I./include -std=c++11 src/log_pow2.cpp -o log_pow2 -Wall -Wextra

#include "common.h"

#include <cstdint>
#include <cstdio>
#include <iostream>

#define CHECK32
//#define CHECK64

namespace {

#ifdef CHECK32
  using sint = int32_t;
  using dint = int64_t;
#elif CHECK64
  using sint = int64_t;
  using dint = __int128_t;
#else
  #error "Missing types."
#endif

auto constexpr ssize = CHAR_BIT * sizeof(sint);

void test(long double l) {

  auto const lower   = static_cast<dint>(AMARU_POW2(dint, ssize) * l);
  auto const upper   = lower + 1;

  std::cout << "    Multiplier = " << dint(upper) << '\n';

  dint min = 0;
  while ((lower * min >> ssize) == (upper * min >> ssize))
    --min;
  ++min; // Minimum is inclusive.

  dint max = 0;
  while ((lower * max >> ssize) == (upper * max >> ssize))
    ++max;

  std::cout << "    Valid on [" << min << ", " << max << "[.\n";
}

} // namespace <anonymous>

int main() {

  try {

    std::cout << "Testing log10_pow2..." << '\n';
    auto const log10_2 = .3010299956639811952137388947244930267681L;
    test(log10_2);

    std::cout << "Testing log5_pow2..." << '\n';
    auto const log5_2 = .4306765580733930506701065687639656320697L;
    test(log5_2);
  }

  catch (std::exception const& e) {
    printf("std::exception thrown: %s.\n", e.what());
  }

  catch (...) {
    printf("Unknown exception thrown.\n");
  }
}
