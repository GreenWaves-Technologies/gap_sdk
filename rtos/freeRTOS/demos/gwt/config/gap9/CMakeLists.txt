add_library(freertos_config_gap9 INTERFACE)

target_include_directories(freertos_config_gap9 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

#target_link_libraries(freertos_config_gap9 PRIVATE freertos_kernel)
#target_link_libraries(freertos_config_gap9 PRIVATE ri5cy-gap9)
