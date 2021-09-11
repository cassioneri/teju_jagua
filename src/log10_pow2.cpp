// g++ -O3 -std=c++20 src/log10_pow2.cpp -o log10_pow2 -Wall -Wextra

#include <cmath>
#include <cstdint>
#include <iostream>

int main() {

  double  const log10_2 = std::log(2.0)/std::log(10.0);
  int64_t const lower   = (uint64_t(1) << 32) * log10_2;
  int64_t const upper   = lower + 1;

  std::cout << "Multiplier = " << upper << '\n';

  int32_t min = 0;
  while ((lower * min >> 32 ) == (upper * min >> 32))
    --min;
  ++min; // Minimum is inclusive.

  std::cout << "Minimum    = " << min << '\n';

  int32_t max = 0;
  while ((lower * max >> 32 ) == (upper * max >> 32))
    ++max;

  std::cout << "Valid on [" << min << ", " << max << "[.\n";
}
