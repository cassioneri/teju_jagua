#include "linear.hpp"

int main() {

  std::cout << "E1\tF\t5^F\tE2\t2^E2-F\tQ\tR\tM\tK\tCHECK\n";

  for (int E1 = E1_min; E1 <= E1_max; ++E1) {

    auto const F      = log10_pow2(E1 - P);

    if (F <= 0)
      continue;

    auto const P5F    = pow5(F);

    if (P5F < 2*P2P)
      continue;

    auto const E2     = E1 - P - 1;
    auto const P2E2MF = pow2(E2 - F);

    auto const Q      = P2E2MF / P5F;
    auto const R      = P2E2MF % P5F;
    auto const [M, K] = linear::get_M_and_K(R, P5F);

    auto const M0_MAX = (2*P2P - 1);
    auto const CHECK  = linear::check(M, K, R, P5F);

    std::cout <<
      E1     << '\t' <<
      F      << '\t' <<
      P5F    << '\t' <<
      E2     << '\t' <<
      P2E2MF << '\t' <<
      Q      << '\t' <<
      R      << '\t' <<
      M      << '\t' <<
      K      << '\t' <<
      CHECK  << '\n';
  }
  return 0;
}
