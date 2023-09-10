#ifndef AMARU_CPP_COMMON_EXCEPTION_H_
#define AMARU_CPP_COMMON_EXCEPTION_H_

#include <stdexcept>

namespace amaru {

/**
 * \brief Amaru's exception.
 */
struct exception_t : std::runtime_error {
  using std::runtime_error::runtime_error;
};

#define AMARU_ASSERT(cond, msg) if (!(cond)) throw exception_t{msg}

} // namespace amaru

#endif // AMARU_CPP_COMMON_EXCEPTION_H_
