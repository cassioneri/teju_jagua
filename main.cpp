#include "linear.hpp"

// g++ -O3 -std=c++20 main.cpp

constexpr bool is_debug = true;

int main() {

  if (is_debug)
    std::cout << "E2\tF\t5^F\tE\t2^E\tQ\tR\tM\tT\tU\tK\tCHECK\n";
  else
    std::cout << "#include \"common.hpp\"\n"
      "U_and_K_t U_and_K[] = {\n";

  constexpr auto E2_max = E0 + static_cast<int>(P2L);

  for (int E2 = E0; E2 <= E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    auto const F   = log10_pow2(E2);
    auto const P5F = pow5(F);

    if (P5F <= 4*P2P)
      continue;

    auto const E       = E2 - 1 - F;
    auto const P2E     = pow2(E);
    auto const Q       = P2E / P5F;
    auto const R       = P2E % P5F;
    auto const M_and_T = linear::get_M_and_T(R, P5F);
    auto const U_and_K = linear::get_U_and_K(R, P5F, M_and_T);
    auto const CHECK   = linear::check(R, P5F, U_and_K);

    if (is_debug)
      std::cout <<
        E2        << '\t' <<
        F         << '\t' <<
        P5F       << '\t' <<
        E         << '\t' <<
        P2E       << '\t' <<
        Q         << '\t' <<
        R         << '\t' <<
        M_and_T.M << '\t' <<
        M_and_T.T << '\t' <<
        U_and_K.U << '\t' <<
        U_and_K.K << '\t' <<
        CHECK     << '\n';
    else
      std::cout <<
        "{ " << U_and_K.U << ",\t" << U_and_K.K << "},\n";
  }

  if (!is_debug)
    std::cout << "{}};\n";

  return 0;
}
