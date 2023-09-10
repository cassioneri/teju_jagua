#ifndef AMARU_OTHER_OTHER_H_
#define AMARU_OTHER_OTHER_H_

#include <dragonbox/dragonbox.h>

namespace amaru {

namespace dragonbox_compact {

using result_float_t = decltype(jkj::dragonbox::to_decimal(float(0),
  jkj::dragonbox::policy::sign::ignore,
  jkj::dragonbox::policy::cache::compact));

result_float_t
to_decimal(float value);

using result_double_t = decltype(jkj::dragonbox::to_decimal(double(0),
  jkj::dragonbox::policy::sign::ignore,
  jkj::dragonbox::policy::cache::compact));

result_double_t
to_decimal(double value);

} // namespace dragonbox_compact

namespace dragonbox_full {

using result_float_t = decltype(jkj::dragonbox::to_decimal(float(0),
  jkj::dragonbox::policy::sign::ignore,
  jkj::dragonbox::policy::cache::full));

result_float_t
to_decimal(float value);

using result_double_t = decltype(jkj::dragonbox::to_decimal(double(0),
  jkj::dragonbox::policy::sign::ignore,
  jkj::dragonbox::policy::cache::full));

result_double_t
to_decimal(double value);

} // namespace dragonbox_full

} // namespace amaru

#endif // AMARU_OTHER_OTHER_H_