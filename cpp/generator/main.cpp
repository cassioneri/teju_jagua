// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/main.cpp
 *
 * Generator's executable.
 */

#include "config.hpp"
#include "generator.hpp"

#include "common/exception.hpp"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

namespace teju {

void
report_usage(const char* const prog) noexcept {
  std::fprintf(stderr, "Usage: %s [OPTION]... CONFIG DIR\n"
    "Generate Tejú Jaguá source files for the given JSON configuration file "
    "CONFIG. The files are saved in directory DIR.\n",
    prog);
}

generator_t
parse(const char* const filename, const char* const dir) {

  std::cout << "Parsing of " << filename << " started.\n";

  std::ifstream file{filename};
  require(file.is_open(), "Cannot open input file");

  auto const data = nlohmann::json::parse(file, nullptr, true, true);

  auto config = data.get<config_t>();
  validate(config);

  std::string directory = dir;
  if (directory.back() != '/')
    directory.append(1, '/');

  std::cout << "Parsing of " << filename << " finished.\n";

  return { std::move(config), std::move(directory) };
}

} // namespace teju

int
main(int const argc, const char* const argv[]) {

  using namespace teju;

  try {

    if (argc == 3) {
      auto const generator = parse(argv[1], argv[2]);
      generator.generate();
      return 0;
    }

    report_error(argv[0], "expected two arguments");
    std::fprintf(stderr, "\n");
    report_usage(argv[0]);

  }

  catch (exception_t const& e) {
    report_error(argv[0], e.what());
  }

  catch (std::exception const& e) {
    report_error(argv[0], e.what());
  }

  catch (...) {
    report_error(argv[0], "unknown error");
  }

  return -1;
}
