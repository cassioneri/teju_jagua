add_library(amaru STATIC
  generated/ieee32_compact.c
  generated/ieee32_full.c
  generated/ieee64_compact.c
  generated/ieee64_full.c
  double.c
  float.c
)

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
