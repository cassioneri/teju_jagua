# SPDX-License-Identifier: APACHE-2.0
# SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

add_executable(benchmark
  main.cpp
)

target_link_libraries(benchmark PRIVATE
  teju
  gtest
  nanobench
  others
)
