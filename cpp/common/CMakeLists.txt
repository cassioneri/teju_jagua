# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_library(others OBJECT
  dragonbox.cpp
)

target_include_directories(others PUBLIC
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(others
  dragonbox::dragonbox
  ryu::ryu
)
