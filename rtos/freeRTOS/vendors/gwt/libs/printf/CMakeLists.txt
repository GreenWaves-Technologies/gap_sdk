
FILE(GLOB SRCS
    printf.c
    )


FILE(GLOB ASM_SRCS
    )

add_library(freertos_printf STATIC ${ASM_SRCS} ${SRCS})

target_include_directories(freertos_printf PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(freertos_printf PRIVATE freertos)

if(NOT "${CMAKE_BUILD_TYPE}" EQUAL "Release")
    target_link_libraries(freertos_printf PRIVATE -lgcc)
else()
    target_compile_options(freertos_printf PRIVATE 
        "-DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_EXPONENTIAL")
endif()

#target_compile_options(freertos_printf PRIVATE -O3)

