# inter-zone-single-stream experiment

set(example_source_files
  inter-zone-single-stream.cc
)

build_lib_example(
    NAME inter-zone-single-stream
    SOURCE_FILES ${example_source_files}
    LIBRARIES_TO_LINK ${libzonal-research}
)


# intra-zone-single-stream experiment

set(example_source_files
  intra-zone-single-stream.cc
)

build_lib_example(
    NAME intra-zone-single-stream
    SOURCE_FILES ${example_source_files}
    LIBRARIES_TO_LINK ${libzonal-research}
)

