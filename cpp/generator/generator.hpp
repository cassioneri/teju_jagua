/**
 * @file cpp/generator/generator.hpp
 *
 * Generator of Amaru's implementations. 
 */

#ifndef AMARU_CPP_GENERATOR_GENERATOR_HPP_
#define AMARU_CPP_GENERATOR_GENERATOR_HPP_

#include "cpp/generator/config.hpp"
#include "cpp/generator/multiprecision.hpp"

#include <cstdint>
#include <string>

namespace amaru {

/**
 * \brief Generator of Amaru's implementation for a given floating point
 * number type.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param info            The information on the floating point number
   *                        type.
   * \param directory       The directory where generated files are saved.
   */
  generator_t(config_t config, std::string directory);

  /**
   * \brief Generates the implementation.
   */
  void
  generate() const;

private:

  struct impl_t;

  impl_t const self() const;

  config_t     config_;
  std::string  prefix_;
  std::string  function_;
  integer_t    mantissa_min_;
  integer_t    mantissa_max_;
  std::int32_t index_offset_;
  std::string  directory_;
  std::string  dot_h_;
  std::string  dot_c_;

}; // struct generator_t

} // namespace amaru

#endif // AMARU_CPP_GENERATOR_GENERATOR_HPP_
