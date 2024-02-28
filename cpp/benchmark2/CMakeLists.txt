# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_executable(benchmark2
  main.cpp
)

target_link_libraries(benchmark2 PRIVATE
  amaru
  gtest gtest_main
  nanobench
  other
)
