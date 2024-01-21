add_library(amaru STATIC
  generated/ieee32.c
  generated/ieee64.c
  generated/ieee128.c
  double.c
  float.c
  float128.c
)

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
