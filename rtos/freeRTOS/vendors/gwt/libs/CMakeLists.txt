FILE(GLOB SRCS
    src/errno.c
    src/stdlib.c
    src/string.c
    )


FILE(GLOB ASM_SRCS
    )

add_library(freertos_libs STATIC ${ASM_SRCS} ${SRCS})

add_subdirectory(printf)

target_include_directories(freertos_libs PUBLIC "include")
target_link_libraries(freertos_libs PRIVATE freertos_gap9)
target_link_libraries(freertos_libs PRIVATE freertos_printf)

