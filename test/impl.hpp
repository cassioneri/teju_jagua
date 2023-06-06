#ifndef AMARU_TEST_IMPL_HPP
#define AMARU_TEST_IMPL_HPP

#include "amaru/config.h"

namespace amaru {
namespace test {

struct built_in_1 {
    static int constexpr value = amaru_built_in_1;
};

struct syntectic_1 {
    static int constexpr value = amaru_syntectic_1;
};

struct built_in_2 {
    static int constexpr value = amaru_built_in_2;
};

struct syntectic_2 {
    static int constexpr value = amaru_syntectic_2;
};

struct built_in_4 {
    static int constexpr value = amaru_built_in_4;
};

} // namespace test
} // namespace amaru

#endif // AMARU_TEST_IMPL_HPP
