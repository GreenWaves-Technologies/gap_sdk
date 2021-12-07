# Driver sources
FILE(GLOB SRCS
    )


FILE(GLOB ASM_SRCS
    )

add_library(pmsis_freertos INTERFACE)

target_include_directories(pmsis_freertos INTERFACE "include")
#target_include_directories(pmsis_freertos INTERFACE "rtos/include")

#target_link_libraries(pmsis_freertos PRIVATE pmsis_api)
#target_link_libraries(pmsis_freertos PRIVATE freertos_kernel)
#target_link_libraries(pmsis_freertos PRIVATE freertos_config_gap9)
#target_link_libraries(pmsis_freertos PRIVATE ri5cy-gap9)

add_subdirectory(rtos)
add_subdirectory(backend)
