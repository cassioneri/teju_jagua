#include "generator/config.hpp"
#include "generator/exception.hpp"
#include "generator/generator.hpp"

#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

namespace amaru {

void
report_usage(FILE* const stream, const char* const prog) noexcept {
  std::fprintf(stream, "Usage: %s [OPTION]... CONFIG DIR\n"
    "Generate Amaru source files for the given JSON configuration file CONFIG. "
    "The files are saved in directory DIR.\n"
    "\n"
    "Options:\n"
    "  --h        shows this message and exits.\n",
    prog);
}

void
report_error(const char* const prog, const char* const msg) noexcept {
  std::fprintf(stderr, "%s: error: %s.\n", prog, msg);
  report_usage(stderr, prog);
  std::exit(-1);
}

generator_t
parse(const char* const filename, const char* const dir) {

  std::ifstream file(filename);
  auto const data = nlohmann::json::parse(file);

  auto config = data.get<config_t>();
  validate(config);

  std::string directory = dir;
  if (directory.back() != '/')
    directory.append(1, '/');

  return { std::move(config), std::move(directory) };
}

} // namespace amaru

int
main(int const argc, const char* const argv[]) {

  using namespace amaru;

  if (argc != 3)
    report_error(argv[0], "expected two arguments");

  try {

    auto const generator = parse(argv[1], argv[2]);
    generator.generate();

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
}
