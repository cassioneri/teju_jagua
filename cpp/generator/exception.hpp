#ifndef AMARU_CPP_GENERATOR_EXCEPTION_H_
#define AMARU_CPP_GENERATOR_EXCEPTION_H_

#include <exception>

namespace amaru {

/**
 * \brief Exception thrown by the generator.
 */
struct exception_t : std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace amaru

#endif // AMARU_CPP_GENERATOR_EXCEPTION_H_
