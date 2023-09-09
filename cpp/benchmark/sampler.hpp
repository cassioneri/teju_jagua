#ifndef AMARU_CPP_BENCHMARK_SAMPLER_H_
#define AMARU_CPP_BENCHMARK_SAMPLER_H_

/**
 * @file cpp/benchmark/sampler.hpp
 *
 * Functionalities to draw sample values used in benchmarks.
 */

#include "cpp/common/traits.hpp"

#include <cstdint>
#include <random>

namespace amaru {

/**
 * @brief Types of population used in benchmarks.
 */
enum class population_t {

  // All integers in the interval [1, N] for some N.
  integer,

  // Random floating point numbers that are equidistant from its neighbours.
  centred,

  // All floating point numbers that are not equidistant from its neighbours.
  uncentred,

  // Mix of centred and uncentred.
  mixed
};

/**
 * @brief Draws uniformly distributed pseudo-random mantissas on the set of
 * all mantissa values but the minimum.
 *
 * @tparam T                The floating point type corresponding to the
 *                          mantissa.
 */
template <typename T>
struct mantissa_provider_t {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * @brief Constructor.
   *
   * @param n_mantissas     The number of mantissas to be generated.
   */
  mantissa_provider_t(u1_t const n_mantissas) :
    n_mantissas_{n_mantissas} {
  }

  /**
   * @brief Checks whether there are still mantissas to generate.
   *
   * @returns \c true when there are no more mantissas to generate.
   */
  bool
  empty() const {
    return n_mantissas_ == 0;
  }

  /**
   * @brief Gets the next pseudo-random mantissa.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next pseudo-random mantissa.
   */
  u1_t
  pop() {
    --n_mantissas_;
    return distribution_(device_);
  }

private:

  using distribution_t = std::uniform_int_distribution<u1_t>;

  static u1_t constexpr mantissa_max_ =
    amaru_pow2(u1_t, traits_t::mantissa_size) - 1;

  u1_t            n_mantissas_;
  std::mt19937_64 device_;
  distribution_t  distribution_ = distribution_t{1, mantissa_max_};

}; // mantissa_provider_t

/**
 * @brief Generic pseudo-random generation of floating point numbers.
 *
 * The generator gets a mantissa value from a provider and loops over the set of
 * all exponents, generating all floating point values with the obtained
 * mantissa. When the exponents are exhausted, the provider is called again and
 * the cycle repeats.
 *
 * @tparam T                The floating point number type.
 * @tparam P                The mantissa provider type.
 */
template <typename T, typename P>
struct generic_sampler_t {

  using traits_t = amaru::traits_t<T>;
  using fields_t = typename traits_t::fields_t;

  /**
   * @brief Constructor.
   *
   * @param provider        The mantissa provider.
   */
  generic_sampler_t(P provider) :
    ieee_    {0, provider.pop()  },
    provider_{std::move(provider)} {
  }

  /**
   * @brief Checks whether there are still floating point numbers to be
   * generated.
   *
   * @returns \c true when there are no more floating point numbers to generate.
   */
  bool
  empty() const {
    return ieee_.exponent == exponent_max_;
  }

  /**
   * @brief Gets the next floating point number.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next generated floating point number.
   */
  T
  pop() {

    auto const value = traits_t::ieee_to_value(ieee_);
    ++ieee_.exponent;

    // Avoids exponent == exponent_max_, since the corresponding value is
    // infinity or NaN.
    if (!provider_.empty() && ieee_.exponent == exponent_max_) {
      ieee_.exponent = 0;
      ieee_.mantissa = provider_.pop();
    }

    return value;
  }

private:

  static constexpr std::int32_t exponent_max_ = amaru_pow2(std::int32_t,
    traits_t::exponent_size) - 1;

  fields_t ieee_;
  P        provider_;

}; // generic_sampler_t

/**
 * @brief Generates floating point numbers.
 *
 * @tparam T                The floating point number type.
 * @tparam population       The type of population.
 */
template <typename T, population_t population>
struct sampler_t;

/**
 * @brief Specialisation of \c sampler_t for \c population_t::integer.
 *
 * The generator generates all strictly positive integers less than or equal to
 * a given upper bound.
 *
 * @tparam T                The floating point number type.
 */
template <typename T>
struct sampler_t<T, population_t::integer> {

  /**
   * @brief Constructor.
   *
   * @pre <tt>bound >= 0</tt>.
   *
   * @param bound           The given upper bound.
   */
  sampler_t(T const bound) : bound_{bound} {
  }

  /**
   * @brief Checks whether there are still floating point numbers to be
   * generated.
   *
   * @returns \c true when there are no more floating point numbers to generate.
   */
  bool
  empty() const {
    return value_ >= bound_;
  }

  /**
   * @brief Gets the next floating point number.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next generated floating point number.
   */
  T
  pop() {
    return value_++;
  }

private:

  T value_{1};
  T bound_{1};

}; // sampler_t<T, population_t::integer>

/**
 * @brief Specialisation of \c sampler_t for \c population_t::centred.
 *
 * The generator gets a mantissa value from a \c mantissa_provider_t and loops
 * over the set of all exponents, generating all floating point values with the
 * obtained mantissa. When the exponents are exhausted, the provider is called
 * again and the cycle repeats.
 *
 * @tparam T                The floating point number type.
 */
template <typename T>
struct sampler_t<T, population_t::centred> {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * @brief Constructor.
   *
   * @param n_mantissas     The number of mantissas to be generated.
   */
  explicit sampler_t(u1_t const n_mantissas) :
    generic_{mantissa_provider_t<T>{n_mantissas}} {
  }

  /**
   * @brief Checks whether there are still floating point numbers to be
   * generated.
   *
   * @returns \c true when there are no more floating point numbers to generate.
   */
  bool
  empty() const {
    return generic_.empty();
  }

  /**
   * @brief Gets the next floating point number.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next generated floating point number.
   */
  T
  pop() {
    return generic_.pop();
  }

private:

  generic_sampler_t<T, mantissa_provider_t<T>> generic_;

}; // sampler_t<T, population_t::centred>

/**
 * @brief Specialisation of \c sampler_t for \c population_t::uncentred.
 *
 * Generates all floating point values which are uncentred.
 *
 * @tparam T                The floating point number type.
 */
template <typename T>
struct sampler_t<T, population_t::uncentred> {

  using traits_t = amaru::traits_t<T>;
  using u1_t   = typename traits_t::u1_t;

  /**
   * @brief Checks whether there are still floating point numbers to be
   * generated.
   *
   * @returns \c true when there are no more floating point numbers to generate.
   */
  bool
  empty() const {
    return generic_.empty();
  }

  /**
   * @brief Gets the next floating point number.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next generated floating point number.
   */
  T
  pop() {
    return generic_.pop();
  }

private:

  /**
   * @brief Provides the minimum mantissa only.
   */
  struct provider_t {

    /**
     * @brief Checks whether there are still mantissas to generate.
     *
     * @returns \c true when there are no more mantissas to generate.
     */
    bool
    empty() const {
      return empty_;
    }

    /**
     * @brief Gets the minimum mantissa.
     *
     * @pre <tt>!empty()</tt>.
     *
     * @returns The minimum mantissa.
     */
    u1_t
    pop() {
      empty_ = true;
      return 0;
    }

  private:
    bool empty_{false};
  };

  generic_sampler_t<T, provider_t> generic_{provider_t{}};

}; // sampler_t<T, population_t::uncentred>

/**
 * @brief Specialisation of \c sampler_t for \c population_t::mixed.
 *
 * The generator gets a mantissa value from a \c mantissa_provider_t and loops
 * over the set of all exponents, generating all floating point values with the
 * obtained mantissa. When the exponents are exhausted, the provider is called
 * again and the cycle repeats. When there are no more mantissas to be provided.
 * from the \c mantissa_provider_t instance, then all uncentred floating point
 * values are generated.
 *
 * @tparam T                The floating point number type.
 */
template <typename T>
struct sampler_t<T, population_t::mixed> {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * @brief Constructor.
   *
   * @param n_mantissas     The number of mantissas to be generated.
   */
  explicit sampler_t(u1_t const n_mantissas) :
    centred_{n_mantissas} {
  }

  /**
   * @brief Checks whether there are still floating point numbers to be
   * generated.
   *
   * @returns \c true when there are no more floating point numbers to generate.
   */
  bool
  empty() const {
    return centred_.empty() && uncentred_.empty();
  }

  /**
   * @brief Gets the next floating point number.
   *
   * @pre <tt>!empty()</tt>.
   *
   * @returns The next generated floating point number.
   */
  T
  pop() {
    return !centred_.empty() ? centred_.pop() : uncentred_.pop();
  }

private:

  sampler_t<T, population_t::centred>   centred_;
  sampler_t<T, population_t::uncentred> uncentred_;

}; // sampler_t<T, population_t::mixed>

} // namespace amaru

#endif // AMARU_CPP_BENCHMARK_SAMPLER_H_
