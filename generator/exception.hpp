#ifndef AMARU_GENERATOR_EXCEPTION_H_
#define AMARU_GENERATOR_EXCEPTION_H_

#include <exception>

namespace amaru {

/**
 * \brief Exception thrown by the generator.
 */
struct exception_t : std::range_error {
  using std::range_error::range_error;
};

} // namespace amaru

#endif // AMARU_GENERATOR_EXCEPTION_H_
