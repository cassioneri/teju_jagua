#ifndef AMARU_TEST_DIV10_DIV10_HPP
#define AMARU_TEST_DIV10_DIV10_HPP

#include <cstdint>

#define amaru_size 32
#define amaru_u1_t std::uint32_t
#define amaru_u2_t std::uint64_t

namespace amaru {
namespace test {

template <int id>
amaru_u1_t div10(amaru_u1_t m);

} // namespace amaru
} // namespace test

#endif // AMARU_TEST_DIV10_DIV10_HPP
