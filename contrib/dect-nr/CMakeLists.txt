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
    model/dect-channel.cc
    model/dect-nr.cc
    model/dect-phy.cc
    model/dect-mac.cc
    model/end-device-dect-nr-mac.cc
    model/dect-net-device.cc
    helper/dect-nr-helper.cc)

set(header_files
    model/dect-channel.h
    model/dect-nr.h
    model/dect-phy.h
    model/dect-mac.h
    model/end-device-dect-nr-mac.h
    model/dect-net-device.h
    helper/dect-nr-helper.h)

build_lib(
    LIBNAME dect-nr
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    LIBRARIES_TO_LINK
            #${libcore}
            ${spectrum}
            ${mobility}
            ${network}
    TEST_SOURCES test/dect-nr-test-suite.cc
                 ${examples_as_tests_sources}
)

