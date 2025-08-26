# SPDX-License-Identifier: APACHE-2.0
# SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

add_library(common OBJECT)

target_sources(common PRIVATE
  src/dragonbox.cpp
  src/traits.cpp
)

target_include_directories(common
  PUBLIC
    "${CMAKE_SOURCE_DIR}/cpp/common/include"
  PRIVATE
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(common PUBLIC
  Boost::multiprecision
  dragonbox::dragonbox
  ryu::ryu
  teju
)
