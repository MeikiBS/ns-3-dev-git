# build_lib_example(
#     NAME dect-nr-example
#     SOURCE_FILES dect-nr-example.cc
#     LIBRARIES_TO_LINK
#             ${libnetwork}        
#             ${libdect-nr}
# )

# build_lib_example(
#   NAME simple-net-device-example
#   SOURCE_FILES simple-net-device-example.cc
#   LIBRARIES_TO_LINK ${libnetwork}
#                     ${libdect-nr}
#                     ${libpoint-to-point}
#                     ${libinternet}
#                     ${libapplications}
# )

build_lib_example(
  NAME dect2020-simulation
  SOURCE_FILES dect2020-simulation.cc
  LIBRARIES_TO_LINK ${libnetwork}
                    ${libdect-nr}
                    ${libpoint-to-point}
                    ${libinternet}
                    ${libapplications}
)

build_lib_example(
  NAME dect2020-simulation1
  SOURCE_FILES dect2020-simulation1.cc
  LIBRARIES_TO_LINK ${libnetwork}
                    ${libdect-nr}
                    ${libpoint-to-point}
                    ${libinternet}
                    ${libapplications}
)

build_lib_example(
  NAME dect2020-simulation2
  SOURCE_FILES dect2020-simulation2.cc
  LIBRARIES_TO_LINK ${libnetwork}
                    ${libdect-nr}
                    ${libpoint-to-point}
                    ${libinternet}
                    ${libapplications}
)