// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/common/exception.hpp"
#include "cpp/generator/config.hpp"
#include "cpp/generator/generator.hpp"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

namespace teju {

void
report_usage(const char* const prog) noexcept {
  std::fprintf(stderr, "Usage: %s [OPTION]... CONFIG DIR\n"
    "Generate Teju Jagua source files for the given JSON configuration file "
    "CONFIG. The files are saved in directory DIR.\n",
    prog);
}

generator_t
parse(const char* const filename, const char* const dir) {

  std::ifstream file{filename};
  require(file.is_open(), "Cannot open input file");

  auto const data = nlohmann::json::parse(file, nullptr, true, true);

  auto config = data.get<config_t>();
  validate(config);

  std::string directory = dir;
  if (directory.back() != '/')
    directory.append(1, '/');

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
