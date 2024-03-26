build_lib_example(
    NAME dect-nr-example
    SOURCE_FILES dect-nr-example.cc
    LIBRARIES_TO_LINK
            ${libnetwork}        
            ${libdect-nr}
)

build_lib_example(
  NAME simple-net-device-example
  SOURCE_FILES simple-net-device-example.cc
  LIBRARIES_TO_LINK ${libnetwork}
                    ${libdect-nr}
                    ${libpoint-to-point}
                    ${libinternet}
                    ${libapplications}
)
