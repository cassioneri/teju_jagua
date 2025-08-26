// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "generator.hpp"
#include "multiprecision.hpp"
#include "splitter.hpp"

#include "common/exception.hpp"
#include "teju/src/common.h"

#include <algorithm>
#include <fstream>

namespace teju {

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------

namespace {

/**
 * @brief Returns the type prefix corresponding to a given width.
 *
 * @param  width            The width.
 *
 * @returns The type prefix corresponding to a given width.
 */
std::string
get_prefix(std::uint32_t const width) {
  switch (width) {
    case 16:
      return "teju16_";
    case 32:
      return "teju32_";
    case 64:
      return "teju64_";
    case 128:
      return "teju128_";
  }
  throw exception_t{"BUG: Unrecognised width."};
}

/**
 * @brief Converts a given string to upper case letters.
 *
 * @param  str              The given string
 *
 * @returns The upper case string.
 */
std::string
to_upper(std::string const& str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result),
    [](char const c) { return static_cast<char>(std::toupper(c)); });
  return result;
}

/**
 * @brief Returns 2^n.
 *
 * @param  n                The exponent n.
 *
 * @returns 2^n.
 */
integer_t
pow2(std::uint32_t const n) {
  return integer_t{1} << n;
}

/**
 * @brief Returns 5^n.
 *
 * @param  n                The exponent n.
 *
 * @returns 5^n.
 */
integer_t
pow5(std::uint32_t const n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

/**
 * @brief Returns the inverse of 5 modulo 2^k.
 *
 * @param  k                The exponent k.
 *
 * @returns The inverse of 5 modulo 2^k
 */
integer_t
minverse5(std::uint32_t k) {
  auto const n    = pow2(k) + 2;
  auto const mask = n - 3; // pow2(k) - 1
  auto m = integer_t{1};
  while (k > 1) {
    m  = (m * (n - ((5 * m) & mask))) & mask;
    k /= 2;
  }
  return m;
}

/**
 * @brief The objective function of the primary maximisation problem:
 *
 *     phi_1(n) := n / (delta_1 - alpha_1 * n % delta_1).
 *
 * @param  alpha_1          Parameter alpha_1.
 * @param  delta_1          Parameter delta_1.
 * @param  n                Variable n.
 *
 * @returns The value of phi_1(n).
 */
rational_t
phi_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& n) {
  return {n, delta_1 - alpha_1 * n % delta_1};
}

/**
 * @brief The objective function of the secondary maximisation problem:
 *
 *     phi_2(n) := n / (1 + (alpha_2 * n - 1) % delta_2).
 *
 * @param  alpha_2          Parameter alpha_2.
 * @param  delta_2          Parameter delta_2.
 * @param  n                Variable n.
 *
 * @returns The value of phi_2(n).
 */
rational_t
phi_2(integer_t const& alpha_2, integer_t const& delta_2,
  integer_t const& n) {
  return {n, 1 + (alpha_2 * n - 1) % delta_2};
}

/**
 * @brief Given alpha_1, delta_1, L_1 and U_1, this function calculates the
 *        maximiser of phi_1(m) over [L_1, U_1].
 *
 * @param  alpha_1          Parameter alpha_1.
 * @param  delta_1          Parameter delta_1.
 * @param  L_1              Lower bound L_1.
 * @param  U_1              Upper bound U_1.
 *
 * @pre 0 <= alpha_1 && alpha_1 < delta_1 && 1 <= L1 && L_1 <= U_1.
 *
 * @returns The maximiser of phi_1(m) over [L_1, U_1].
 */
rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& L_1, integer_t const& U_1);

/**
 * @brief Given alpha, delta, L_2 and U_2, this function calculates the
 *        maximiser of phi_2(m) over [L_2, U_2].
 *
 * @param  alpha_2          Parameter alpha_1.
 * @param  delta_2          Parameter delta_1.
 * @param  L_2              Lower bound L_2.
 * @param  U_2              Upper bound U_2.
 *
 * @pre 0 < alpha_2 && 0 < delta_2 && 1 <= L_2 && L_2 <= U_2.
 *
 * @returns The maximiser of phi_2(m) over [L_2, U_2].
 */
rational_t
get_maximum_2(integer_t const& alpha_2, integer_t const& delta_2,
  integer_t const& L_2, integer_t const& U_2);

rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& L_1, integer_t const& U_1) {

  auto const maximum_1  = phi_1(alpha_1, delta_1, U_1);

  if (alpha_1 == 0 || L_1 == U_1)
    return maximum_1;

  auto const L_2 = alpha_1 * L_1 / delta_1 + 1;
  auto const U_2 = alpha_1 * U_1 / delta_1;

  if (L_2 == U_2 + 1)
    return maximum_1;

  auto const  alpha_2 = delta_1 % alpha_1;
  auto const& delta_2 = alpha_1;
  auto const  other   = get_maximum_2(alpha_2, delta_2, L_2, U_2);

  auto const  maximum_2 = rational_t{
    delta_1 * numerator(other) - denominator(other),
      alpha_1 * denominator(other)};

  return std::max(maximum_1, maximum_2);
}

rational_t
get_maximum_2(integer_t const& alpha_2,
  integer_t const& delta_2, integer_t const& L_2, integer_t const& U_2) {

  if (alpha_2 == 0)
    return U_2;

  auto const maximum_1 = phi_2(alpha_2, delta_2, L_2);

  if (L_2 == U_2)
    return maximum_1;

  auto const L_1 = (alpha_2 * L_2 - 1) / delta_2 + 1;
  auto const U_1 = (alpha_2 * U_2 - 1) / delta_2;

  if (L_1 == U_1 + 1)
    return maximum_1;

  auto const  alpha1 = delta_2 % alpha_2;
  auto const& delta1 = alpha_2;
  auto const  other  = get_maximum_1(alpha1, delta1, L_1, U_1);

  auto const  maximum_2 = rational_t{
    delta_2 * numerator(other) + denominator(other),
    alpha_2 * denominator(other)};

  return std::max(maximum_1, maximum_2);
}

} // namespace <anonymous>

//------------------------------------------------------------------------------
// generator_t
//------------------------------------------------------------------------------

generator_t::generator_t(config_t config, std::string directory) :
  config_      {std::move(config)              },
  prefix_      {get_prefix(width())            },
  function_    {"teju_" + id()                 },
  mantissa_min_{pow2(mantissa_width() - 1u)    },
  mantissa_max_{pow2(mantissa_width()) - 1u    },
  index_offset_{teju_log10_pow2(exponent_min())},
  directory_   {std::move(directory)           },
  dot_h_       {id() + ".h"                    },
  dot_c_       {id() + ".c"                    } {
}

void
generator_t::generate() const {

  auto const p2width      = pow2(width());
  auto       dot_h_stream = std::ofstream{directory() + dot_h()};
  auto       dot_c_stream = std::ofstream{directory() + dot_c()};

  std::cout << "Generation started.\n";

  std::cout << "  Generating \"" << dot_h() << "\".\n";
  generate_dot_h(dot_h_stream);

  std::cout << "  Generating \"" << dot_c() << "\".\n";
  generate_dot_c(dot_c_stream);

  std::cout << "Generation finished.\n";
}

std::string const&
generator_t::id() const {
  return config_.id;
}

std::uint32_t
generator_t::width() const {
  return config_.width;
}

std::string const&
generator_t::spdx_identifier() const {
  return config_.spdx.identifier;
}

std::vector<std::string> const&
generator_t::spdx_copyright() const {
  return config_.spdx.copyright;
}

std::string const&
generator_t::prefix() const {
  return prefix_;
}

std::string const&
generator_t::function() const {
  return function_;
}

std::int32_t
generator_t::exponent_min() const {
  return config_.exponent.minimum;
}

std::int32_t
generator_t::exponent_max() const {
  return config_.exponent.maximum;
}

std::uint32_t
generator_t::mantissa_width() const {
  return config_.mantissa.width;
}

integer_t const&
generator_t::mantissa_min() const {
  return mantissa_min_;
}

integer_t const&
generator_t::mantissa_max() const {
  return mantissa_max_;
}

std::uint32_t
generator_t::storage_split() const {
  return config_.storage.split;
}

std::string const&
generator_t::storage_endianness() const {
  return config_.storage.endianness;
}

std::int32_t
generator_t::index_offset() const {
  return index_offset_;
}

std::string const&
generator_t::calculation_div10() const {
  return config_.calculation.div10;
}

std::string const&
generator_t::calculation_mshift() const {
  return config_.calculation.mshift;
}

std::string const&
generator_t::directory() const {
  return directory_;
}

std::string const&
generator_t::dot_h() const {
  return dot_h_;
}

std::string const&
generator_t::dot_c() const {
  return dot_c_;
}

std::ostream&
generator_t::generate_license(std::ostream& stream) const {

  stream <<
    "// SPDX-License-Identifier: " << spdx_identifier() << '\n';

  for (auto const& copyright : spdx_copyright())
    stream <<
      "// SPDX-FileCopyrightText: " << copyright << '\n';

  return stream << '\n';
}

void
generator_t::generate_dot_h(std::ostream& stream) const {

  std::string const include_guard = "TEJU_TEJU_GENERATED_" + to_upper(id()) +
    "_H_";

  generate_license(stream) <<
    "// This file was generated. DO NOT EDIT IT.\n"
    "\n"
    "#ifndef " << include_guard << "\n"
    "#define " << include_guard << "\n"
    "\n"
    "#include \"teju/src/config.h\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n" << prefix() << "fields_t\n" <<
    function() << '(' << prefix() << "fields_t binary);\n"
    "\n" <<
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n"
    "\n"
    "#endif // " << include_guard <<
    "\n";
}

void
generator_t::generate_dot_c(std::ostream& stream) const {

  // Performs the overflow safety checks except for uncentred refined
  // calculations. The latter check is delayed to after checking whether the
  // uncentred case is sorted because, if so, then Tejú Jaguá never performs
  // uncentred refined calculations.
  require(check_centred_calculations(), "Centred calculations could overflow.");
  require(check_uncentred_calculations(),
    "Uncentred calculations could overflow.");

  generate_license(stream) <<
    "// This file was generated. DO NOT EDIT IT.\n"
    "\n"
    "#include \"./" << dot_h() << "\"\n"
    "\n"
    "#include \"teju/src/literal.h\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n";

  stream <<
    "#define teju_width                " << width()          << "u\n"
    "#define teju_exponent_min         " << exponent_min()   << "\n"
    "#define teju_mantissa_width       " << mantissa_width() << "u\n"
    "#define teju_storage_index_offset " << index_offset()   << "\n";

  if (!calculation_div10().empty()) {

    if (calculation_div10() == "built_in_2" ||
      calculation_div10() == "synthetic_1") {
      require(check_div10_algorithm(),
        "Can't use the selected algorithm for div10.");
    }
    stream <<
    "#define teju_calculation_div10    teju_" << calculation_div10() << "\n";
  }

  // The optimal runtime shift is twice the carrier width because it avoids
  // teju_mshift to work on partial limbs.
  auto const shift = 2 * width();

  auto const upper_str = "upper";
  auto const lower_str = "lower";
  auto const is_little = storage_endianness() == "little";
  auto const first     = is_little ? lower_str : upper_str;
  auto const last      = is_little ? upper_str : lower_str;

  stream <<
    "#define teju_calculation_mshift   teju_" << calculation_mshift() << "\n"
    "\n"
    "#define teju_function             " << function() << "\n"
    "#define teju_fields_t             " << prefix()   << "fields_t\n"
    "#define teju_u1_t                 " << prefix()   << "u1_t\n"
    "\n"
    "#if defined(" << prefix() << "u2_t)\n"
    "  #define teju_u2_t               " << prefix()   << "u2_t\n"
    "#endif\n"
    "\n"
    "#if defined(" << prefix() << "u4_t)\n"
    "  #define teju_u4_t               " << prefix()   << "u4_t\n"
    "#endif\n"
    "\n"
    "typedef struct {\n"
    "  teju_u1_t const " << first << ";\n"
    "  teju_u1_t const " << last  << ";\n"
    "} teju_multiplier_t;\n"
    "\n"
    "static const teju_multiplier_t multipliers[] = {\n";

  auto const p2width  = pow2(width());
  auto const mask     = p2width - 1;
  auto const splitter = splitter_t{width(), storage_split()};
  bool       sorted   = true;

  auto const get_e_0  = [](int32_t const e) {
    return e - int32_t(teju_log10_pow2_residual(e));
  };

  auto const e_0_min  = get_e_0(exponent_min());
  auto const e_0_max  = get_e_0(exponent_max());

  for (auto e_0 = e_0_min; e_0 <= e_0_max; e_0 = get_e_0(e_0 + 4)) {

    auto U = get_fast_eaf_numerator(e_0, e_0 == e_0_min);

    sorted &= std::invoke([&]{
      auto const m_a = 4 * mantissa_min() - 1;
      auto const a   = m_a * U >> (shift + 1);
      auto const m_b = 2 * mantissa_min() + 1;
      auto const b   = m_b * U >> shift;
      return a < b;
    });

    // Output

    integer_t       upper = U >> width();
    integer_t       lower = std::move(U &= mask);
    int32_t   const f     = teju_log10_pow2(e_0);

    require(upper < p2width, "A multiplier is out of range.");

    stream << "  { " <<
      (is_little ? splitter(std::move(lower)) : splitter(std::move(upper))) <<
      ", " <<
      (is_little ? splitter(std::move(upper)) : splitter(std::move(lower))) <<
      " }, // " << std::dec << f << '\n';
  }

  require(sorted || check_uncentred_refined_calculations(),
    "Uncentred refined calculation could overflow.");

  stream << "};\n"
    "\n"
    "#define teju_calculation_sorted " << sorted << "u\n"
    "\n"
    "static struct {\n"
    "  teju_u1_t const multiplier;\n"
    "  teju_u1_t const bound;\n"
    "} const minverse[] = {\n";

  // Let M = mantissa_max(). Tejú Jaguá might call is_multiple_of_pow5(n, f) for
  // the following values of  n:
  //
  //   Centred case:
  //     m_a = (( 2 * m - 1) << r)                      <= ( 2 * M - 1) * 8;
  //     m_b = (( 2 * m + 1) << r)                      <= ( 2 * M + 1) * 8;
  //     c_2 = (( 4 * m    ) << r) * 2^(e_0 - 1) / 10^f <  ( 4 * M    ) * 8.
  //
  //   Uncentred case:
  //     c_2 = (( 4 * m    ) << r) * 2^(e_0 - 1) / 10^f <  ( 4 * M    ) * 8.
  //
  //   Uncentred case, refined:
  //     c_2 = ((40 * m    ) << r) * 2^(e_0 - 1) / 10^f <  (40 * M    ) * 8.
  //
  // Hence, n < 320 * M. Now, if 5^f >= 320 * M, then n < 5^f. It follows that
  // n is not multiple of 5^f, that is, is_multiple_of_pow5(n, f) == false.

  auto const bound      = 320 * mantissa_max();
  auto const minv5      = minverse5(width());
  auto       multiplier = integer_t{1};
  auto       p5         = integer_t{1};
  for (std::int32_t f = 0; p5 < bound; ++f) {

    auto bnd = p2width / p5 - (f == 0);

    stream << "  { " << splitter(multiplier) << ", " <<
      splitter(std::move(bnd)) << " }, // " << std::dec << f << '\n';

    multiplier *= minv5;
    multiplier &= mask;
    p5         *= 5;
  }

  stream << std::dec <<
    "};\n"
    "\n"
    "#include \"teju/src/teju.h\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n";
}

bool
generator_t::check_div10_algorithm() const {

  // Check whether Tejú Jaguá can use the algorithm of Theorem 4 of Neri C,
  // Schneider L. "Euclidean affine functions and their application to calendar
  // algorithms." Softw Pract Exper. 2023; 53(4):937-970.
  // https://onlinelibrary.wiley.com/doi/full/10.1002/spe.3172

  auto const d       = integer_t{10};
  auto const k       = width();
  auto const p2k     = pow2(k);
  auto const a       = p2k / d + 1;
  auto const epsilon = d - p2k % d;
  auto const U       = ((a + epsilon - 1) / epsilon) * d - 1;
  // b = ((2 * m + 1) << r) * 2^(e_0 - 1) / 10^f
  //   < ((2 * mantissa_max() + 1) << 3) * 1
  //   = 16 * mantissa_max() + 8.
  auto const b_max   = 16 * mantissa_max() + 8;
  return epsilon <= a && b_max < U;
}

bool
generator_t::check_centred_calculations() const {
  // Calculations of m_a, m_b and m_c are safe if the carrier type can represent
  // m_c = (4u * m << r) for all values of m and r, i.e.,
  //   (4 * mantissa_max() << 3) <  2^width() <=>
  //   32 * mantissa_max()       <  2^width().
  // In terms of number of bits, the above is equivalent to
  //    5 + mantissa_width()     <= width().
  return 5 + mantissa_width() <= width();
}

bool
generator_t::check_uncentred_calculations() const {
  // Calculations of m_a and m_b are safe if the carrier type can represent
  // m_a = (4u * m - 1u) << r for m = mantissa_min() and all values of r, i.e.,
  //   (4 * mantissa_min()  - 1) << 3 <  2^width() <=>
  //   32 * mantissa_min()  - 8       <  2^width() <=>
  // In terms of number of bits, the above is equivalent to
  //    5 + mantissa_width() - 1      <= width()
  return 4 + mantissa_width() <= width();
}

bool
generator_t::check_uncentred_refined_calculations() const {
  // Calculation of m_c is safe if the carrier type can represent
  // m_c = 40u * m << r for m = mantissa_min() and all values of r, i.e.,
  //   40 * mantissa_min() << 3     <  2^width() <=>
  //  320 * mantissa_min()          <  2^width() <=>
  // In terms of number of bits, the above is equivalent to
  //     9 + (mantissa_width() - 1) <= width()
  return 8 + mantissa_width() <= width();
}

integer_t
generator_t::get_fast_eaf_numerator(int32_t const e_0, bool const is_min)
  const {

  auto const shift = 2 * width();
  auto const f     = teju_log10_pow2(e_0);

  integer_t alpha, delta;
  if (f <= 0) {
    alpha = pow5(-f);
    delta = pow2(-(e_0 - 1 - f));
  }
  else {
    alpha = pow2(e_0 - 1 - f);
    delta = pow5(f);
  }

  auto const maximum = get_maximum(alpha, delta, is_min);

  integer_t q, r;
  divide_qr(alpha << shift, delta, q, r);

  require(maximum < rational_t{pow2(shift), delta - r},
    "Unable to use shift that is twice the width.");

  return q + 1;
}

rational_t
generator_t::get_maximum(integer_t alpha, integer_t const& delta,
  bool const is_min) const {

  alpha %= delta;

  // Usual interval for the centred case.

  auto const L = is_min ? integer_t{1} : integer_t{2 * mantissa_min() + 1};
  auto const U = integer_t{(4 * mantissa_max()) << 3};

  auto const max_LU = get_maximum_1(alpha, delta, L, U);

  // Extras for the uncentred case.

  auto const extras = [&](uint32_t const r) {

    auto const m_a               = ( 4 * mantissa_min() - 1) << r;
    auto const m_b               = ( 2 * mantissa_min() + 1) << r;
    auto const m_c               = ( 4 * mantissa_min()    ) << r;
    auto const m_c_refined       = (40 * mantissa_min()    ) << r;

    auto const phi_1_m_a         = phi_1(alpha, delta, m_a);
    auto const phi_1_m_b         = phi_1(alpha, delta, m_b);
    auto const phi_1_m_c         = phi_1(alpha, delta, m_c);
    auto const phi_1_m_c_refined = phi_1(alpha, delta, m_c_refined);

    return std::max({phi_1_m_a, phi_1_m_a, phi_1_m_c, phi_1_m_c_refined});
  };

  return std::max({max_LU, extras(0), extras(1), extras(2), extras(3)});
}

} // namespace teju
