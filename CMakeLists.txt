check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/zonal-research-examples-test-suite.cc
        )
endif()

set(source_files
  model/processing-bridge-net-device.cc
  helper/zonal-layout-helper.cc
  helper/processing-bridge-helper.cc
  helper/stream-latency-trace-helper.cc
)

set(header_files
  model/processing-bridge-net-device.h
  helper/zonal-layout-helper.h
  helper/processing-bridge-helper.h
  helper/stream-latency-trace-helper.h
)

build_lib(
    LIBNAME zonal-research
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    LIBRARIES_TO_LINK ${libcore} 
      ${libapplications}
      ${libinternet}
      ${libnetwork}
      ${libbridge}
      ${libcsma}
      ${libpoint-to-point}
    TEST_SOURCES ${examples_as_tests_sources}
)

