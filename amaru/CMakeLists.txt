add_library(amaru STATIC
  generated/ieee32_compact.c
  generated/ieee64_compact.c
  double_compact.c
  float_compact.c
)

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
