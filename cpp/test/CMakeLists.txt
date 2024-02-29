# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_executable(test

  # Tests
  div10.cpp
  log.cpp
  main.cpp
  mshift.cpp

  # Helpers
  test_case.cpp

  # Several realisations of div10 and mshift for testing.
  built_in_1.cpp
  built_in_1_small_shift.cpp
  built_in_2.cpp
  built_in_4.cpp
  synthetic_1.cpp
  synthetic_1_small_shift.cpp
  synthetic_2.cpp
  synthetic_2_small_shift.cpp
)

target_link_libraries(test
  teju
  boost_multiprecision
  gtest gtest_main
  other
)
