#ifndef AMARU_BENCHMARK_TRAITS_H_
#define AMARU_BENCHMARK_TRAITS_H_

/**
 * @file benchmark/traits.hpp
 *
 * Common interface for treatment of different floating points types.
 */

#include "amaru/double.h"
#include "amaru/float.h"
#include "other/other.hpp"

#include <cstdint>

namespace amaru {

/**
 * \brief Traits for floating point numbers.
 *
 * \tparam T                The floating point number type.
 */
template <typename T>
struct traits_t;

namespace detail {

  /**
   * \brief Returns the IEEE-754 floating point value corresponding to the
   * given mantissa and exponent.
   *
   * \tparam T              The floating point number type.
   * \tparam mantissa_size  The number of bits in the mantissa.
   * \tparam U              An unsigned integer type of the same size as \c T.
   */
  template <typename T, std::uint32_t mantissa_size, typename U>
  T
  from_ieee(U exponent, U mantissa) {

    static_assert(sizeof(T) == sizeof(U), "Incompatible types");

    auto const bits = (exponent << mantissa_size) | mantissa;
    T value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
  }

} // namespace detail

// Specialisation of traits_t for double.
template <>
struct traits_t<float> {

  using u1_t = std::uint32_t;

  static auto constexpr exponent_size = std::uint32_t{8};
  static auto constexpr mantissa_size = std::uint32_t{23};

  static
  void
  amaru_compact(float const value) {
    amaru_from_float_to_decimal_compact(value);
  }

  static
  void
  amaru_full(float const value) {
    amaru_from_float_to_decimal_full(value);
  }

  static
  void
  dragonbox_compact(float const value) {
    amaru::dragonbox_compact::to_decimal(value);
  }

  static void
  dragonbox_full(float const value) {
    amaru::dragonbox_full::to_decimal(value);
  }

  static
  float
  from_ieee(std::uint32_t exponent, std::uint32_t mantissa) {
    return detail::from_ieee<float, mantissa_size>(exponent, mantissa);
  }

  static
  amaru32_fields_t
  to_ieee(float const value) {
    return amaru_from_float_to_fields(value);
  }

}; // traits_t<float>

// Specialisation of traits_t for double.
template <>
struct traits_t<double> {

  using u1_t = std::uint64_t;

  static auto constexpr exponent_size = std::uint32_t{11};
  static auto constexpr mantissa_size = std::uint32_t{52};

  static
  void
  amaru_compact(double const value) {
    amaru_from_double_to_decimal_compact(value);
  }

  static
  void
  amaru_full(double const value) {
    amaru_from_double_to_decimal_full(value);
  }

  static
  void
  dragonbox_compact(double const value) {
    amaru::dragonbox_compact::to_decimal(value);
  }

  static
  void
  dragonbox_full(double const value) {
    amaru::dragonbox_full::to_decimal(value);
  }

  static
  double
  from_ieee(std::uint64_t exponent, std::uint64_t mantissa) {
    return detail::from_ieee<double, mantissa_size>(exponent, mantissa);
  }

  static
  amaru64_fields_t
  to_ieee(double const value) {
    return amaru_from_double_to_fields(value);
  }

}; // traits_t<double>

} // namespace amaru

#endif // AMARU_BENCHMARK_TRAITS_H_
