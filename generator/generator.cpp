#include "amaru/common.h"
#include "generator/generator.hpp"
#include "generator/exception.hpp"

#include <algorithm>
#include <fstream>

namespace amaru {

namespace {

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

/**
 * \brief Calculates 2^n.
 */
integer_t
pow2(std::uint32_t const n) {
  return integer_t{1} << n;
}

/**
 * \brief Returns 5^n.
 */
integer_t
pow5(std::uint32_t const n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

/**
 * \brief The objective function of the primary maximisation problem:
 *
 *     phi_1(m) := m / (delta_1 - alpha_1 * m % delta_1).
 */
rational_t
phi_1(integer_t const& alpha_1, integer_t const& delta_1, integer_t const& m) {
  return {m, delta_1 - alpha_1 * m % delta_1};
}

/**
 * \brief The objective function of the secondary maximisation problem:
 *
 *     phi_2(m) := m / (1 + (alpha_2 * m - 1) % delta_2).
 */
rational_t
phi_2(integer_t const& alpha_2, integer_t const& delta_2, integer_t const& m) {
  return {m, 1 + (alpha_2 * m - 1) % delta_2};
}

/**
 * \brief Given alpha_1, delta_1, a_1 and b_1, this function calculates the
 * maximiser of phi_1(m) over [a_1, b_1[.
 *
 * \pre 0 <= alpha_1 && alpha_1 < delta_1 && a_1 < b_1.
 */
rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& a_1, integer_t const& b_1);

/**
 * \brief Given alpha, delta, a_2 and b_2, this function calculates the
 * maximiser of phi_2(m) over [a_2, b_2[.
 *
 * \pre 0 < alpha_2 && 0 < delta_2 && 1 <= a_2 && a_2 < b_2.
 */
rational_t
get_maximum_2(integer_t const& alpha_2, integer_t const& delta_2,
  integer_t const& a_2, integer_t const& b_2);

rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& a_1, integer_t const& b_1) {

  auto const b_minus_1 = b_1 - 1;
  auto const maximum1  = phi_1(alpha_1, delta_1, b_minus_1);

  if (alpha_1 == 0 || a_1 == b_minus_1)
    return maximum1;

  auto const a_2 = alpha_1 * a_1 / delta_1 + 1;
  auto const b_2 = alpha_1 * b_minus_1 / delta_1 + 1;

  if (a_2 == b_2)
    return maximum1;

  auto const  alpha_2 = delta_1 % alpha_1;
  auto const& delta_2 = alpha_1;
  auto const  other   = get_maximum_2(alpha_2, delta_2, a_2, b_2);

  auto const  maximum2 = rational_t{
    delta_1 * numerator(other) - denominator(other),
      alpha_1 * denominator(other)};

  return std::max(maximum1, maximum2);
}

rational_t
get_maximum_2(integer_t const& alpha_2,
  integer_t const& delta_2, integer_t const& a_2, integer_t const& b_2) {

  if (alpha_2 == 0)
    return b_2 - 1;

  auto const maximum1 = phi_2(alpha_2, delta_2, a_2);

  if (a_2 == b_2 - 1)
    return maximum1;

  auto const a1 = (alpha_2 * a_2 - 1) / delta_2 + 1;
  auto const b1 = (alpha_2 * (b_2 - 1) - 1) / delta_2 + 1;

  if (a1 == b1)
    return maximum1;

  auto const  alpha1 = delta_2 % alpha_2;
  auto const& delta1 = alpha_2;
  auto const  other  = get_maximum_1(alpha1, delta1, a1, b1);

  auto const  maximum2 = rational_t{
    delta_2 * numerator(other) + denominator(other),
    alpha_2 * denominator(other)};

  return std::max(maximum1, maximum2);
}

/**
 * \brief Get the fields type corresponding to a given size.
 *
 * \param The size
 *
 * \returns The fields type corresponding to the given size.
 */
std::string
get_fields(uint32_t const size) {
  switch (size) {
    case 32:
      return "amaru32_fields_t";
    case 64:
      return "amaru64_fields_t";
  }

  // Should never get here since size is previously validated.
  return {};
}

/**
 * \brief Converts a given string to upper case letters.
 *
 * \param str The given string
 *
 * \returns The upper case string.
 */
std::string
to_upper(std::string const& str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result),
    [](char const c) { return static_cast<char>(std::toupper(c)); });
  return result;
}

/**
 * \brief Fast EAF coefficients.
 *
 * For given alpha > 0 and delta > 0, we often find U > 0 and k >= 0 such that
 *     alpha * m / delta = U * m >> k for m in a certain interval.
 *
 * This type stores U and k.
 */
struct fast_eaf_t {
  integer_t     U;
  std::uint32_t k;
};

/**
 * \brief Stores alpha, delta (usually pow2(e) and pow2(f)) and the maximum of
 *     m / (delta - alpha * m % delta)
 * for m in the set of mantissas.
 */
struct alpha_delta_maximum_t {
  integer_t  alpha;
  integer_t  delta;
  rational_t maximum;
};

std::int32_t
get_index_offset(base_t const base, std::int32_t const exponent_min) {
  return base == base_t::decimal ? log10_pow2(exponent_min) : exponent_min;
}

}

struct generator_t::impl_t {

  impl_t(generator_t const* generator);

  /**
   * \brief Returns the identifier for the floating point number type.
   */
  std::string const&
  id() const;

  /**
   * \brief Returns the size of the limb in bits.
   */
  std::uint32_t
  size() const;

  /**
   * \brief Returns the C/C++ name of Amaru's fields type.
   */
  std::string const&
  fields() const;

  /**
   * \brief Returns the name of Amaru's conversion function.
   */
  std::string const&
  function() const;

  /**
   * \brief Returns the minimum binary exponent.
   */
  std::int32_t
  exponent_min() const;

  /**
   * \brief Returns maximum binary exponent.
   */
  std::int32_t
  exponent_max() const;

  /**
   * \brief Returns the size of mantissa in bits.
   */
  std::uint32_t
  mantissa_size() const;

  /**
   * \brief Returns the normal (inclusive) minimal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_min() const;

  /**
   * \brief Returns the normal (exclusive) maximal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_max() const;

  /**
   * \brief Returns the number of storage limbs.
   */
  std::uint32_t
  storage_limbs() const;

  /**
   * \brief Returns the storage exponent.
   */
  base_t
  storage_base() const;

  /**
   * \brief Returns the number of storage limbs.
   */
  std::int32_t
  index_offset() const;

  /**
   * \brief Returns whether using a compact table of multipliers.
   */
  bool
  is_compact() const;

  /**
   * \brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const;

  /**
   * \brief Returns the name of the generated .h file.
   */
  std::string const&
  dot_h() const;

  /**
   * \brief Returns the name of the generated .c file.
   */
  std::string const&
  dot_c() const;

  /**
   * \brief Streams out the .h file.
   *
   * \param stream Output stream to receive the content.
   */
  void
  generate_dot_h(std::ostream& stream) const;

  /**
   * \brief Streams out the .c file.
   *
   * \param stream Output stream to receive the content.
   */
  void
  generate_dot_c(std::ostream& stream) const;

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size
   *     exponent_max() - exponent_min() + 1
   * such that v[i] contains the maximum of the primary problem corresponding to
   * exponent = exponent_min() + i.
   *
   * \returns The vector v.
   */
  std::vector<alpha_delta_maximum_t>
  get_maxima() const;

  /**
   * \brief Given alpha and delta, this function calculates the maximiser of
   * phi(m) over the relevant set of mantissas.
   *
   * \pre 0 <= alpha && 0 < delta.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1 = false)
    const;

  /**
   * \brief Get the EAF f(m) = alpha * m / delta which works on an interval of
   * relevant mantissas. This fast EAF is associated to maximisation of phi(m)
   * over the set of mantissas.
   *
   * \param x               The container of alpha, beta and the solution of
   *                        the primary maximisation problem.
   */
  fast_eaf_t
  get_fast_eaf(alpha_delta_maximum_t const& x) const;

  generator_t const& self;
};

generator_t::impl_t const
generator_t::self() const {
  return impl_t(this);
}

generator_t::generator_t(config_t config, std::string directory) :
  config_      {std::move(config)                            },
  fields_      {get_fields(self().size())                    },
  function_    {"amaru_binary_to_decimal_" + self().id()     },
  mantissa_min_{AMARU_POW2(integer_t, self().mantissa_size())},
  mantissa_max_{2 * self().mantissa_min()                    },
  index_offset_{get_index_offset(self().storage_base(),
    self().exponent_min())                                   },
  directory_   {std::move(directory)                         },
  dot_h_       {self().id() + ".h"                           },
  dot_c_       {self().id() + ".c"                           } {
}

void
generator_t::generate() const {

  auto const p2_size      = integer_t{1} << self().size();
  auto       dot_h_stream = std::ofstream{self().directory() + self().dot_h()};
  auto       dot_c_stream = std::ofstream{self().directory() + self().dot_c()};

  std::cout << "Generation started.\n";

  // Overflow check 1:
  if (2 * self().mantissa_max() + 1 >= p2_size)
    throw exception_t("The limb is not large enough for calculations to "
      "not overflow.");

  // Overflow check 2:
  if (20 * self().mantissa_min() >= p2_size)
    throw exception_t("The limb is not large enough for calculations to "
      "not overflow.");

  std::cout << "  Generating \"" << self().dot_h() << "\".\n";
  self().generate_dot_h(dot_h_stream);

  std::cout << "  Generating \"" << self().dot_c() << "\".\n";
  self().generate_dot_c(dot_c_stream);

  std::cout << "Generation finished.\n";
}

generator_t::impl_t::impl_t(generator_t const* self) :
  self{*self} {
}

std::string const&
generator_t::impl_t::id() const {
  return self.config_.id;
}

std::uint32_t
generator_t::impl_t::size() const {
  return self.config_.size;
}

std::string const&
generator_t::impl_t::fields() const {
  return self.fields_;
}

std::string const&
generator_t::impl_t::function() const {
  return self.function_;
}

std::int32_t
generator_t::impl_t::exponent_min() const {
  return self.config_.exponent.minimum;
}

std::int32_t
generator_t::impl_t::exponent_max() const {
  return self.config_.exponent.maximum;
}

std::uint32_t
generator_t::impl_t::mantissa_size() const {
  return self.config_.mantissa.size;
}

integer_t const&
generator_t::impl_t::mantissa_min() const {
  return self.mantissa_min_;
}

integer_t const&
generator_t::impl_t::mantissa_max() const {
  return self.mantissa_max_;
}

std::uint32_t
generator_t::impl_t::storage_limbs() const {
  return self.config_.storage.limbs;
}

base_t
generator_t::impl_t::storage_base() const {
  return self.config_.storage.base;
}

std::int32_t
generator_t::impl_t::index_offset() const {
  return self.index_offset_;
}

bool
generator_t::impl_t::is_compact() const {
  return self.config_.storage.base == base_t::decimal;
}

std::string const&
generator_t::impl_t::directory() const {
  return self.directory_;
}

std::string const&
generator_t::impl_t::dot_h() const {
  return self.dot_h_;
}

std::string const&
generator_t::impl_t::dot_c() const {
  return self.dot_c_;
}

void
generator_t::impl_t::generate_dot_h(std::ostream& stream) const {

  std::string const include_guard = "AMARU_AMARU_GENERATED_" + to_upper(id()) +
    "_H_";

  stream <<
    "// This file was auto-generated. DO NOT EDIT IT.\n"
    "\n"
    "#ifndef " << include_guard << "\n"
    "#define " << include_guard << "\n"
    "\n"
    "#include \"amaru/types.h\"\n"
    "\n"
    "#include <stdbool.h>\n"
    "#include <stdint.h>\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n" <<
      fields() << ' ' << function() << "(bool is_negative, "
      "int32_t exponent, amaru" << size() << "_limb1_t mantissa);\n"
    "\n" <<
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n"
    "\n"
    "#endif // " << include_guard <<
    "\n";
}

void
generator_t::impl_t::generate_dot_c(std::ostream& stream) const {

  stream << "// This file was auto-generated. DO NOT EDIT IT.\n"
    "\n" <<
    "#include \"" << dot_h() << "\"\n"
    "\n"
    "#ifdef __cplusplus\n"
    "extern \"C\" {\n"
    "#endif\n"
    "\n"
    "#define AMARU_MAX_LIMBS AMARU" << size() << "_MAX_LIMBS\n"
    "\n"
    "typedef amaru" << size() << "_limb1_t amaru_limb1_t;\n"
    "#if AMARU" << size() << "_MAX_LIMBS >= 2\n"
    "typedef amaru" << size() << "_limb2_t amaru_limb2_t;\n"
    "#elif AMARU" << size() << "_MAX_LIMBS >= 4\n"
    "typedef amaru" << size() << "_limb4_t amaru_limb4_t;\n"
    "#endif\n"
    "\n"
    "typedef amaru" << size() << "_fields_t amaru_fields_t;\n"
    "\n";

  auto const maxima = get_maxima();
  std::vector<fast_eaf_t> fast_eafs;
  fast_eafs.reserve(maxima.size());

  auto shift = uint32_t{0};

  // Calculates minimal fast EAFs (i.e., those with minimal shift).

  for (auto const& x : maxima) {
    fast_eafs.emplace_back(get_fast_eaf(x));
    auto const s = fast_eafs.back().k;
    if (s > shift)
      shift = s;
  }

  // Optimal shift is 2 * size since it prevents multipliy_and_shift to
  // deal with partial limbs. In addition to subtract 1 to compensate the
  // increment adjustment made when the shift is output.
  if (is_compact())
    shift = 2 * size() - 1;

  // Replace minimal fast EAFs to use the same shift.

  auto const p2shift = integer_t{1} << shift;

  for (std::uint32_t i = 0; i < maxima.size(); ++i) {

    auto const& x = maxima[i];

    integer_t q, r;
    divide_qr(x.alpha << shift, x.delta, q, r);

    if (x.maximum >= rational_t{p2shift, x.delta - r})
      throw exception_t{"Unable to use same shift."};

    fast_eafs[i] = fast_eaf_t{q + 1, shift};
  }

  auto const p2_size = integer_t{1} << size();

  stream <<
    "static amaru_data_t const amaru_data = {\n"
    "  /* size: */ "           << size()          << ",\n"
    "  /* exponent: */ {\n"
    "    /* minimum: */ "      << exponent_min()  << "\n"
    "  },\n"
    "  /* mantissa: */ {\n"
    "    /* size: */ "         << mantissa_size() << "\n"
    "  },\n"
    "  /* storage: */ {\n"
    "    /* limbs: */ "        << storage_limbs() << ",\n"
    "    /* is_compact: */ "   << is_compact()    << ",\n"
    "    /* index_offset: */ " << index_offset()  << "\n"
    "  },\n"
    "  /* calculation: */ {\n"
    // Instead of Amaru dividing multipliy_and_shift(m_a, upper, lower) by 2
    // we increment the shift here so this has the same effect.
    "    /* shift: */ "        << shift + 1 << "\n"
    "  }\n"
    "};\n"
    "\n";

  stream <<
    "enum {\n"
    "  is_compact    = " << is_compact()    << ",\n"
    "  size          = " << size()          << ",\n"
    "  mantissa_size = " << mantissa_size() << ",\n"
    "  exponent_min  = " << exponent_min()  << ",\n"
    "  index_offset  = " << index_offset()  << ",\n"
    // Instead of Amaru dividing multipliy_and_shift(m_a, upper, lower) by 2
    // we increment the shift here so this has the same effect.
    "  shift         = " << shift + 1       << "\n"
    "};\n"
    "\n";

  stream <<
    "static struct {\n"
    "  amaru_limb1_t const upper;\n"
    "  amaru_limb1_t const lower;\n"
    "} const multipliers[] = {\n";

  auto const nibbles = size() / 4;

  auto e2      = exponent_min();
  auto e2_or_f = is_compact() ? log10_pow2(e2) : e2;

  for (auto const& fast_eaf : fast_eafs) {

    integer_t upper, lower;
    divide_qr(fast_eaf.U, p2_size, upper, lower);

    if (upper >= p2_size)
      throw exception_t{"Multiplier is out of range."};

    stream << "  { " <<
      "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << upper <<
      ", "
      "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << lower <<
      std::dec << " }, // " << e2_or_f << "\n";
    ++e2_or_f;
  }

  stream << "};\n"
    "\n"
    "static struct {\n"
    "  amaru_limb1_t const multiplier;\n"
    "  amaru_limb1_t const bound;\n"
    "} const minverse[] = {\n";

  auto const minverse5  = integer_t{p2_size - (p2_size - 1) / 5};
  auto multiplier = integer_t{1};
  auto p5 = integer_t{1};

  // Amaru checks whether is_multiple_of_pow5(C, f) for
  // 1. C  = 2 * mantissa + 1 <= 2 * mantissa_max + 1;
  // 2. C <= 2 * mantissa * 2^e * 5^{-f} <= 20 * mantissa_max;
  // 3. C  = 20 * mantissa_min * 2^e * 5^{-f} <= 200 * mantissa_min;
  // Hence, 200 * mantissa_max is a conservative bound, i.e.,
  // If 5^f > 200 * mantissa_max, then is_multiple_of_pow5(C, f) == false;
  for (int32_t f = 0; p5 <= 200 * mantissa_max(); ++f) {
    auto const bound = p2_size / p5 - (f == 0);
    stream << "  { "
      "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
      multiplier << ", " <<
      "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
      bound <<
      " },\n";
    multiplier = (multiplier * minverse5) % p2_size;
    p5 *= 5;
  }

  stream << "};\n"
    "\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n"
    "\n"
    "#define AMARU_FUNCTION " << function() << "\n"
    "#include \"amaru/amaru.h\"\n"
    "\n"
    "#undef AMARU_FUNCTION\n"
    "#undef AMARU_MAX_LIMBS\n";
}

std::vector<alpha_delta_maximum_t>
generator_t::impl_t::get_maxima() const {

  std::vector<alpha_delta_maximum_t> maxima;
  maxima.reserve(exponent_max() - exponent_min() + 1);

  auto f_done = log10_pow2(exponent_min()) - 1;

  for (auto e2 = exponent_min(); e2 <= exponent_max(); ++e2) {

    auto const f = log10_pow2(e2);

    if (is_compact() && f == f_done)
      continue;

    auto const e = (is_compact() ? e2 - log10_pow2_remainder(e2) : e2) -
      f;

    alpha_delta_maximum_t x;
    x.alpha   = f >= 0 ? pow2(e) : pow5(-f);
    x.delta   = f >= 0 ? pow5(f) : pow2(-e);
    x.maximum = get_maximum(x.alpha, x.delta, e2 == exponent_min());

    maxima.emplace_back(std::move(x));

    f_done = f;
  }
  return maxima;
}

rational_t
generator_t::impl_t::get_maximum(integer_t alpha, integer_t const& delta,
  bool const start_at_1) const {

  alpha %= delta;

  // Usual interval.

  auto const a = start_at_1 ? integer_t{1} : integer_t{2 * mantissa_min()};
  auto const b = is_compact() ? integer_t{16 * mantissa_max() - 15} :
    integer_t{2 * mantissa_max()};

  auto const max_ab = get_maximum_1(alpha, delta, a, b);

  // Extras that are needed when mantissa == normal_mantissa_min().

  auto max_extras = [&](auto const& mantissa) {
    auto const m_a     =  4 * mantissa - 1;
    auto const m_c     = 20 * mantissa;
    auto const max_m_a = phi_1(alpha, delta, m_a);
    auto const max_m_c = phi_2(alpha, delta, m_c);
    return std::max(max_m_a, max_m_c);
  };

  if (!is_compact())
    return std::max(max_ab, max_extras(mantissa_min()));

  return std::max({max_ab, max_extras(mantissa_min()),
    max_extras(2 * mantissa_min()), max_extras(4 * mantissa_min()),
    max_extras(8 * mantissa_min())});
}

fast_eaf_t
generator_t::impl_t::get_fast_eaf(alpha_delta_maximum_t const& x) const {

  // Making shift >= size, simplifies multiply_and_shift executed at runtime.
  // Indeed, it ensures that the least significant limb of the product is
  // irrelevant. For this reason, later on, the generator actually outputs
  // shift - size (still labelling it as 'shift') so that Amaru doesn't need
  // to do it at runtime.
  auto k    = size();
  auto pow2 = integer_t{1} << k;

  integer_t q, r;
  divide_qr(pow2 * x.alpha, x.delta, q, r);

  // It should return from inside the loop but let's set an upper bound.
  while (k < 3 * size()) {

    if (x.maximum < rational_t{pow2, x.delta - r})
      return { q + 1, k };

    k    += 1;
    pow2 *= 2;
    q    *= 2;
    r    *= 2;
    while (r >= x.delta) {
      q += 1;
      r -= x.delta;
    }
  }

  throw exception_t{"Cannot find fast EAF."};
}

} // namespace amaru
