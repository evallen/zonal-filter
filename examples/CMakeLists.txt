set(example_source_files
  zonal-latency-measurement.cc
  zonal-latency-measurement.h
)

build_lib_example(
    NAME zonal-latency-measurement
    SOURCE_FILES ${example_source_files}
    LIBRARIES_TO_LINK ${libzonal-research}
)

