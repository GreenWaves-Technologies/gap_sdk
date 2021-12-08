
FILE(GLOB SRCS
    port.c
    )


FILE(GLOB ASM_SRCS
    port_asm.S
    )


add_library(ri5cy-gap9 STATIC ${ASM_SRCS} ${SRCS})

#target_link_libraries(ri5cy-gap9 PRIVATE freertos)
target_link_libraries(ri5cy-gap9 PRIVATE freertos_kernel)
target_link_libraries(ri5cy-gap9 PRIVATE freertos_gap9)
target_link_libraries(ri5cy-gap9 PRIVATE freertos_config_gap9)
target_link_libraries(ri5cy-gap9 PRIVATE pmsis_implem_gap9)
target_link_libraries(ri5cy-gap9 PRIVATE pmsis_freertos)
target_link_libraries(ri5cy-gap9 PRIVATE freertos)
target_include_directories(ri5cy-gap9 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
