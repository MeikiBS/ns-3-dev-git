check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/dect-nr-examples-test-suite.cc
        )
endif()

set(source_files
    model/dect2020-net-device.cc
    model/dect2020-mac.cc
    model/dect2020-phy.cc
    model/dect2020-channel.cc
    model/dect2020-beacon-header.cc
    model/dect2020-beacon-message.cc
    model/dect2020-mac-header-type.cc
    model/dect2020-operating-band.cc
    model/dect2020-spectrum-signal-parameters.cc)

set(header_files
    model/dect2020-net-device.h
    model/dect2020-mac.h
    model/dect2020-phy.h
    model/dect2020-channel.h
    model/dect2020-beacon-header.h
    model/dect2020-beacon-message.h
    model/dect2020-mac-header-type.h
    model/dect2020-operating-band.h
    model/dect2020-spectrum-signal-parameters.h)

build_lib(
    LIBNAME dect-nr
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    LIBRARIES_TO_LINK
            ${libspectrum}
            ${spectrum}
            ${mobility}
            ${network}
    TEST_SOURCES test/dect-nr-test-suite.cc
                 ${examples_as_tests_sources}
)
