build_lib_example(
    NAME dect-nr-example
    SOURCE_FILES dect-nr-example.cc
    LIBRARIES_TO_LINK
            ${libnetwork}        
            ${libdect-nr}
)
