cmake_minimum_required(VERSION 3.13)

if(CONFIG_USE_PREBUILT)
    include(${CONFIG_GAP_SDK_HOME}/utils/cmake/macros.cmake)
    setupcrosscompile()
endif()

project(freertos C ASM)

set(SRCS
    freertos_kernel/list.c
    freertos_kernel/queue.c
    freertos_kernel/tasks.c
    freertos_kernel/timers.c
    freertos_kernel/event_groups.c
    freertos_kernel/stream_buffer.c
    demos/gwt/config/gap9/FreeRTOS_util.c
    )

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)


set(freertos_flags  "-D__USE_TCDM_MALLOC__=1"
                    "-DPMSIS_DRIVERS=1"
                    "-D__FC_MALLOC_NATIVE__=0"
                    "-D__L2_MALLOC_NATIVE__=0"
                    "-D__PMSIS_L2_MALLOC_NATIVE__=0"
                    "-D__riscv__"
                    "-D__GAP__"
                    "-D__RISCV_ARCH_GAP__=1"
                    "-D__FREERTOS__"
                    "-DFEATURE_CLUSTER=1"
                    "-DCONFIG_GAP9_V2"
                    "-D__PLATFORM_GVSOC__"
                    "-D__PLATFORM__=ARCHI_PLATFORM_GVSOC"
                    "-DPRINTF_RTL"
                    "-DMAIN_APP_STACK_SIZE=2048"
                    "-D__SEMIHOSTING__"
                    "-DPRINTF_SEMIHOST"
)

# set(freertos_flags "-D__USE_TCDM_MALLOC__=1" "-DPMSIS_DRIVERS=1" "-D__FC_MALLOC_NATIVE__=0" "-D__L2_MALLOC_NATIVE__=0" "-D__PMSIS_L2_MALLOC_NATIVE__=0" "-D__riscv__" "-D__GAP__" "-D__RISCV_ARCH_GAP__=1" "-D__FREERTOS__" "-DFEATURE_CLUSTER=1" "-DCONFIG_GAP9_V2" "-std=gnu99")
# list(APPEND freertos_flags "-D__PLATFORM_GVSOC__")
# list(APPEND freertos_flags "-D__PLATFORM__=ARCHI_PLATFORM_GVSOC")
# list(APPEND freertos_flags "-DPRINTF_RTL")
# list(APPEND freertos_flags "-DMAIN_APP_STACK_SIZE=2048")


set(CLOCK_FLAGS "-DCONFIG_FAST_OSC_FREQUENCY=${CONFIG_FAST_OSC_FREQUENCY}"
                "-DCONFIG_FLL_MAXDCO_FREQ=${CONFIG_FLL_MAXDCO_FREQ}"
                "-DCONFIG_FREQUENCY_PERIPH=${CONFIG_FREQUENCY_PERIPH}"
                "-DCONFIG_FREQUENCY_FC=${CONFIG_FREQUENCY_FC}"
                "-DCONFIG_FREQUENCY_CLUSTER=${CONFIG_FREQUENCY_CLUSTER}"
                "-DCONFIG_FREQUENCY_SFU=${CONFIG_FREQUENCY_SFU}"
                "-DCONFIG_FREQUENCY_FPGA=${CONFIG_FREQUENCY_FPGA}"
                        )

# Compiler flags
set(GCC_FLAGS   "-g"
                "-fmessage-length=0"
                "-fno-exceptions"
                "-ffunction-sections"
                "-fdata-sections"
                "-funsigned-char"
                "-fno-delete-null-pointer-checks"
                "-fomit-frame-pointer"
                "-Os"
                "-std=gnu99"
                "-fno-jump-tables"
)

set(common_flags    ${BOARD_FLAGS}
                    ${GCC_FLAGS} 
                    "-D__GAP__" 
                    "-D__RISCV_ARCH_GAP__=1" 
                    "-D__FREERTOS__" 
                    "-DFEATURE_CLUSTER=1"
)

set(asm_flags   ${ARCHI_FLAGS}
                ${common_flags} 
                "-DASSEMBLY_LANGUAGE -x assembler-with-cpp"
)

set(ld_script ${CMAKE_CURRENT_SOURCE_DIR}/vendors/gwt/gap9/src/device/ld/GAP9.ld)
set(linker_opt -nostartfiles -nostdlib -T${ld_script} -MD -MP -MT  ${ARCHI_FLAGS} -Wl,--gc-sections,-Map=$@.map,-static,-lgcc -lgcc )

add_subdirectory(vendors/gwt)
add_subdirectory(demos/gwt/config/gap9)
add_subdirectory(freertos_kernel)
add_subdirectory(freertos_kernel/portable)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pmsis ${CMAKE_CURRENT_BINARY_DIR}/pmsis)

set_source_files_properties(${GCC_CRT_OBJS} PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
add_library(freertos STATIC ${ASM_SRCS} ${SRCS} )

# Set compiling option for ASM language sources of freertos target.
target_compile_options(freertos PUBLIC  $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>)

#Set compiling option for C language sources of freertos target.
target_compile_options(freertos PUBLIC $<$<COMPILE_LANGUAGE:C>: ${freertos_flags}
                                                                ${common_flags}
                                                                ${ARCHI_FLAGS}
                                                                ${CLOCK_FLAGS}>)
target_link_options(freertos PUBLIC ${linker_opt})
message("Linker option ${linker_opt}")
target_include_directories(freertos PUBLIC "freertos_kernel/include")

target_link_libraries(freertos PUBLIC ri5cy-gap9)
target_link_libraries(freertos PUBLIC freertos_gap9)
target_link_libraries(freertos PUBLIC pmsis_implem_gap9)
target_link_libraries(freertos PUBLIC pmsis_freertos)
target_link_libraries(freertos PUBLIC freertos_libs)
target_link_libraries(freertos PUBLIC pmsis_api)
target_link_libraries(freertos PUBLIC pmsis_implem)
target_link_libraries(freertos PUBLIC bsp)

target_link_libraries(freertos PUBLIC freertos_printf)
target_link_libraries(freertos PUBLIC freertos_kernel)
target_link_libraries(freertos PUBLIC freertos_config_gap9)
target_link_libraries(freertos PUBLIC pmsis_rtos)
target_link_libraries(freertos PUBLIC freertos_pmsis_backend)

message(STATUS CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})


list(APPEND FREERTOS_TARGET_LIST    freertos
                                    ri5cy-gap9
                                    freertos_gap9
                                    pmsis_implem_gap9
                                    pmsis_freertos
                                    freertos_libs
                                    pmsis_api
                                    pmsis_implem
                                    freertos_printf
                                    freertos_kernel
                                    freertos_config_gap9
                                    pmsis_rtos
                                    freertos_pmsis_backend
)

# BSP may be optional
if($ENV{BSP_ENABLED})
    list(APPEND FREERTOS_TARGET_LIST bsp)
endif()

install(TARGETS ${FREERTOS_TARGET_LIST} 
        EXPORT freertosTargets)

export(EXPORT freertosTargets
     FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/freertosTargets.cmake"
     NAMESPACE freertos::
     )
