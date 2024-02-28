# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_executable(generator
  config.cpp
  generator.cpp
  main.cpp
  splitter.cpp
)

target_include_directories(generator PRIVATE
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(generator PRIVATE
  boost_multiprecision
  nlohmann_json::nlohmann_json
)
