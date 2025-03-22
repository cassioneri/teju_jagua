// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "teju/common.h"
#include "cpp/common/exception.hpp"
#include "cpp/generator/generator.hpp"
#include "cpp/generator/multiprecision.hpp"
#include "cpp/generator/splitter.hpp"

#include <algorithm>
#include <fstream>
#include <limits>

namespace teju {

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------

namespace {

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
  auto const mask = n - 3; // pow2(s) - 1
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
 * maximiser of phi_2(m) over [L_2, U_2].
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

/**
 * @brief Returns the type prefix corresponding to a given size.
 *
 * @param  size             The size.
 *
 * @returns The type prefix corresponding to a given size.
 */
std::string
get_prefix(std::uint32_t const size) {
  switch (size) {
    case 16:
      return "teju16_";
    case 32:
      return "teju32_";
    case 64:
      return "teju64_";
    case 128:
      return "teju128_";
  }
  throw exception_t{"BUG: Unrecognised size."};
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

} // namespace <anonymous>

//------------------------------------------------------------------------------
// generator_t
//------------------------------------------------------------------------------

generator_t::generator_t(config_t config, std::string directory) :
  config_      {std::move(config)              },
  prefix_      {get_prefix(size())             },
  function_    {"teju_" + id()                 },
  mantissa_min_{pow2(mantissa_size() - 1u)     },
  mantissa_max_{2 * mantissa_min()             },
  index_offset_{teju_log10_pow2(exponent_min())},
  directory_   {std::move(directory)           },
  dot_h_       {id() + ".h"                    },
  dot_c_       {id() + ".c"                    } {
}

void
generator_t::generate() const {

  auto const p2size       = pow2(size());
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
generator_t::size() const {
  return config_.size;
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
generator_t::mantissa_size() const {
  return config_.mantissa.size;
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

/**
 * @brief Fast EAF coefficients.
 *
 * For given alpha > 0 and delta > 0, we often find U > 0 and k >= 0 such that:
 *
 *   alpha * m / delta = U * m >> k for m in a certain interval.
 *
 * This type stores U and k.
 */
struct generator_t::fast_eaf_t {
  integer_t     U;
  std::uint32_t k;
};

/**
 * @brief Stores alpha, delta (usually pow2(e) and pow2(f)) and the maximum of
 *        m / (delta - alpha * m % delta) for m in the set of mantissas.
 */
struct generator_t::alpha_delta_maximum_t {
  integer_t  alpha;
  integer_t  delta;
  rational_t maximum;
};

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
    "#include <stdbool.h>\n"
    "#include <stdint.h>\n"
    "\n"
    "#include \"teju/config.h\"\n"
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

  generate_license(stream) <<
    "// This file was generated. DO NOT EDIT IT.\n"
    "\n"
    "#include \"" << dot_h() << "\"\n"
    "\n"
    "#include \"teju/literal.h\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n";

  auto const maxima = get_maxima();
  std::vector<fast_eaf_t> fast_eafs;
  fast_eafs.reserve(maxima.size());

  auto shift = std::uint32_t{0};

  // Calculates minimal fast EAFs (i.e., those with minimal shift).

  for (auto const& x : maxima) {
    fast_eafs.emplace_back(get_fast_eaf(x));
    auto const s = fast_eafs.back().k;
    if (s > shift)
      shift = s;
  }

  // Optimal shift is 2 * size since it prevents mshift to deal with partial
  // limbs. In addition, we subtract 1 to compensate shift's increment made
  // later on, when shift is output. (See below.)
  shift = std::max(shift, 2 * size() - 1);

  // Replace minimal fast EAFs to use the same shift.

  auto const p2shift = pow2(shift);

  for (std::uint32_t i = 0; i < maxima.size(); ++i) {

    auto const& x = maxima[i];

    integer_t q, r;
    divide_qr(x.alpha << shift, x.delta, q, r);

    require(x.maximum < rational_t{p2shift, x.delta - r},
      "Unable to use same shift.");

    fast_eafs[i] = fast_eaf_t{q + 1, shift};
  }

  // Check whether the uncentred case is always sorted, that is, a < b for all
  // exponents.
  auto const m_a = 4 * mantissa_min() - 1;
  auto const m_b = 2 * mantissa_min() + 1;
  bool sorted = true;
  {
    for (auto const& fast_eaf :fast_eafs) {
      // Recall that, at this point, shift == 2 * size() - 1, which is 1 less
      // than it should be and it's only later (below) that it's adjusted.
      auto const a = m_a * fast_eaf.U >> (shift + 2);
      auto const b = m_b * fast_eaf.U >> (shift + 1);
      if (b <= a) {
        sorted = false;
        break;
      }
    }
  }

  auto const p2size   = pow2(size());
  auto const mask     = p2size - 1;
  auto const minv5    = minverse5(size());
  auto const splitter = splitter_t{size(), storage_split()};

  stream <<
    "#define teju_size                 " << size()          << "u\n"
    "#define teju_exponent_min         " << exponent_min()  << "\n"
    "#define teju_mantissa_size        " << mantissa_size() << "u\n"
    "#define teju_storage_index_offset " << index_offset()  << "\n"
    "#define teju_calculation_sorted   " << sorted          << "u\n";

  if (!calculation_div10().empty())
    stream <<
    "#define teju_calculation_div10    teju_" << calculation_div10() << "\n";

  stream <<
    "#define teju_calculation_mshift   teju_" << calculation_mshift() << "\n"
    // Instead of using teju_mshift(m, upper, lower) / 2 in Teju Jagua, shift is
    // incremented here and the division by 2 is removed.
    "#define teju_calculation_shift    " << shift + 1  << "u\n"
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
    "static struct {\n"
    "  teju_u1_t const upper;\n"
    "  teju_u1_t const lower;\n"
    "} const multipliers[] = {\n";

  auto e2 = exponent_min();
  auto f  = teju_log10_pow2(e2);

  for (auto const& fast_eaf : fast_eafs) {

    integer_t upper = fast_eaf.U >> size();
    integer_t lower = fast_eaf.U & mask;

    require(upper < p2size, "Multiplier is out of range.");

    stream << "  { " << splitter(std::move(upper)) << ", " <<
      splitter(std::move(lower)) << " }, // " << std::dec << f << "\n";

    ++f;
  }

  stream << "};\n"
    "\n"
    "static struct {\n"
    "  teju_u1_t const multiplier;\n"
    "  teju_u1_t const bound;\n"
    "} const minverse[] = {\n";

  auto multiplier = integer_t{1};
  auto p5         = integer_t{1};

  // Let M = mantissa_max(). Teju Jagua might call is_multiple_of_pow5(n, f) for
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

  auto const bound = 320 * mantissa_max();
  for (std::int32_t f = 0; p5 < bound; ++f) {

    auto bound = p2size / p5 - (f == 0);

    stream << "  { " << splitter(multiplier) << ", " <<
      splitter(std::move(bound)) << " },\n";

    multiplier = (multiplier * minv5) & mask;
    p5 *= 5;
  }

  stream << std::dec <<
    "};\n"
    "\n"
    "#include \"teju/teju.h\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n";
}

std::vector<generator_t::alpha_delta_maximum_t>
generator_t::get_maxima() const {

  auto const f_min = teju_log10_pow2(exponent_min());
  auto const f_max = teju_log10_pow2(exponent_max());

  std::vector<alpha_delta_maximum_t> maxima;
  maxima.reserve(f_max - f_min + 1);

  auto f_done = f_min - 1;

  for (auto e = exponent_min(); e <= exponent_max(); ++e) {

    auto const f = teju_log10_pow2(e);

    if (f == f_done)
      continue;

    auto const e0_f = e - int32_t(teju_log10_pow2_residual(e)) - f;

    alpha_delta_maximum_t x;
    x.alpha   = f >= 0 ? pow2(e0_f) : pow5(-f   );
    x.delta   = f >= 0 ? pow5(f   ) : pow2(-e0_f);
    x.maximum = get_maximum(x.alpha, x.delta, e == exponent_min());

    maxima.emplace_back(std::move(x));

    f_done = f;
  }
  return maxima;
}

rational_t
generator_t::get_maximum(integer_t alpha, integer_t const& delta,
  bool const start_at_1) const {

  alpha %= delta;

  // Usual interval.

  auto const L = start_at_1 ? integer_t{1} : integer_t{2 * mantissa_min()};
  auto const U = integer_t{16 * mantissa_max() - 15};

  auto const max_LU = get_maximum_1(alpha, delta, L, U);

  // Extras that are needed when mantissa == mantissa_min().

  auto max_extras = [&](auto const& mantissa) {
    auto const m_a     =  4 * mantissa - 1;
    auto const m_c     = 20 * mantissa;
    auto const max_m_a = phi_1(alpha, delta, m_a);
    auto const max_m_c = phi_2(alpha, delta, m_c);
    return std::max(max_m_a, max_m_c);
  };

  return std::max({max_LU, max_extras(mantissa_min()),
    max_extras(2 * mantissa_min()), max_extras(4 * mantissa_min()),
    max_extras(8 * mantissa_min())});
}

generator_t::fast_eaf_t
generator_t::get_fast_eaf(alpha_delta_maximum_t const& x) const {

  // Making shift >= size, simplifies mshift executed at runtime. Indeed, it
  // ensures that the least significant limb of the product is irrelevant. For
  // this reason, later on, the generator actually outputs shift - size
  // (still labelling it as 'shift') so that Teju Jagua doesn't need to do it at
  // runtime.
  auto k   = size();
  auto p2k = pow2(k);

  integer_t q, r;
  divide_qr(p2k * x.alpha, x.delta, q, r);

  // It should return from inside the loop but let's set an upper bound.
  while (k < 3 * size()) {

    if (x.maximum < rational_t{p2k, x.delta - r})
      return { q + 1, k };

    k   += 1;
    p2k *= 2;
    q   *= 2;
    r   *= 2;
    while (r >= x.delta) {
      q += 1;
      r -= x.delta;
    }
  }

  throw exception_t{"Cannot find fast EAF."};
}

} // namespace teju
