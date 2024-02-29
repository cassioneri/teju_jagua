# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_executable(benchmark
  main.cpp
)

target_link_libraries(benchmark PRIVATE
  teju
  gtest gtest_main
  nanobench
  other
)
