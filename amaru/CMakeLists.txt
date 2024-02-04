add_library(amaru STATIC
  generated/ieee32.c
  float.c
  double.c
)

if (AMARU_HAS_FLOAT128)
  target_sources(amaru PUBLIC
    generated/ieee64_with_uint128.c
    generated/ieee128.c
    float128.c
  )
else ()
  target_sources(amaru PUBLIC
    generated/ieee64_no_uint128.c
  )
endif ()

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
