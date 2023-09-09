add_library(amaru STATIC
  generated/ieee32_compact.c
  generated/ieee32_full.c
  generated/ieee64_compact.c
  generated/ieee64_full.c
  generated/ieee128_compact.c
  generated/ieee128_full.c
  double.c
  float.c
  float128.c
)

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
