#ifndef AMARU_OTHER_OTHER_H_
#define AMARU_OTHER_OTHER_H_

#include <dragonbox/dragonbox.h>

namespace amaru {
namespace dragonbox {

using result_float_t = decltype(jkj::dragonbox::to_decimal(float(0)));

result_float_t
to_decimal(float value);

using result_double_t = decltype(jkj::dragonbox::to_decimal(double(0)));

result_double_t
to_decimal(double value);

} // namespace dragonbox
} // namespace amaru

#endif // AMARU_OTHER_OTHER_H_
