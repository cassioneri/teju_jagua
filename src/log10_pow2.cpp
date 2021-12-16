// g++ -O3 -std=c++20 src/log10_pow2.cpp -o log10_pow2 -Wall -Wextra

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iostream>

void test(long double l) {

  auto const lower   = __uint128_t((__uint128_t(1) << 64) * l);
  auto const upper   = lower + 1;

  std::cout << "    Multiplier = " << uint64_t(upper) << '\n';

  int32_t min = 0;
  while ((lower * min >> 64 ) == (upper * min >> 64))
    --min;
  ++min; // Minimum is inclusive.

  int32_t max = 0;
  while ((lower * max >> 64 ) == (upper * max >> 64))
    ++max;

  std::cout << "    Valid on [" << min << ", " << max << "[.\n";
}

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
