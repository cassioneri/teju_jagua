set -ex

presets="
  clang.debug.make
  clang.debug.ninja
  clang.release.make
  clang.release.ninja
  clang.release-symbols.make
  clang.release-symbols.ninja
  gcc.debug.make
  gcc.debug.ninja
  gcc.release.make
  gcc.release.ninja
  gcc.release-symbols.make
  gcc.release-symbols.ninja
"

for preset in ${presets}
  do
    cmake --preset ${preset}
    cmake --build build/${preset}
    if [[ ${preset} == *".release.ninja" ]]; then
      ./build/${preset}/bin/test
    fi
  done
