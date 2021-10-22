# interface library for headers
add_library(gvsoc_gap_headers INTERFACE)
target_include_directories(gvsoc_gap_headers INTERFACE "models")

add_subdirectory(models)
