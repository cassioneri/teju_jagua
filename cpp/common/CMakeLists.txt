# SPDX-License-Identifier: APACHE-2.0
# SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

add_library(others OBJECT
  dragonbox.cpp
  traits.cpp
)

target_include_directories(others PUBLIC
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(others
  Boost::multiprecision
  dragonbox::dragonbox
  ryu::ryu
)
