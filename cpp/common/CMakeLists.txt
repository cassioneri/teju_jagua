# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_library(other OBJECT
  dragonbox.cpp
)

target_include_directories(other PUBLIC
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(other
  dragonbox::dragonbox
)
