#ifndef AMARU_GENERATOR_GENERATOR_HPP_
#define AMARU_GENERATOR_GENERATOR_HPP_

#include "generator/config.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <cstdint>
#include <string>

namespace amaru {

/**
 * \brief Generator of Amaru's implementation for a given floating point number
 * type.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param info            The information on the floating point number type.
   * \param config          The implementation configuration.
   */
  generator_t(config_t config, std::string directory);

  /**
   * \brief Generates the declaration and implementation.
   */
  void
  generate() const;

private:

  using integer_t = boost::multiprecision::cpp_int;

  struct impl_t;

  impl_t const self() const;

  config_t     config_;
  std::string  directory_;
  std::string  function_;
  std::string  fields_;
  std::int32_t dec_exponent_min_;
  integer_t    normal_mantissa_min_;
  integer_t    normal_mantissa_max_;
  integer_t    p2_size_;
  std::string  dot_h_;
  std::string  dot_c_;

}; // struct generator_t

} // namespace amaru

#endif // AMARU_GENERATOR_GENERATOR_HPP_
