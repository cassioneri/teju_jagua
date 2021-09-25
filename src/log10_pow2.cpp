// g++ -O3 -std=c++20 src/log10_pow2.cpp -o log10_pow2 -Wall -Wextra

#include <cmath>
#include <cstdint>
#include <iostream>

int main() {

  auto const log10_2 = std::log(2.0)/std::log(10.0);
  auto const lower   = __uint128_t((__uint128_t(1) << 64) * log10_2);
  auto const upper   = lower + 1;

  std::cout << "Multiplier = " << uint64_t(upper) << '\n';

  int32_t min = 0;
  while ((lower * min >> 64 ) == (upper * min >> 64))
    --min;
  ++min; // Minimum is inclusive.

  std::cout << "Minimum    = " << min << '\n';

  int32_t max = 0;
  while ((lower * max >> 64 ) == (upper * max >> 64))
    ++max;

  std::cout << "Valid on [" << min << ", " << max << "[.\n";
}
