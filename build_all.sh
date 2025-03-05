set -ex

presets="
  clang.release.make
  clang.release.ninja
  clang.release-symbols.make
  clang.release-symbols.ninja
  gcc.release.make
  gcc.release.ninja
  gcc.release-symbols.make
  gcc.release-symbols.ninja
"

for preset in ${presets}
  do
    cmake --preset ${preset}
    cmake --build build/${preset}
    ./build/${preset}/bin/test
  done
