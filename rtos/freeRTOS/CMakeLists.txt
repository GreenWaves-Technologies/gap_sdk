cmake_minimum_required(VERSION 3.13)

if(CONFIG_USE_PREBUILT)
    include(${CONFIG_GAP_SDK_HOME}/utils/cmake/macros.cmake)
    setupcrosscompile()
endif()

project(freertos C ASM)

if(CONFIG_CHIP)
    set(CHIP ${CONFIG_CHIP})
elseif(DEFINED ENV{TARGET_CHIP_FAMILY})
    set(CHIP $ENV{TARGET_CHIP_FAMILY})
else()
    set(CHIP "GAP9")
endif()

if(CONFIG_CHIP_VERSION)
    set(TARGET_CHIP_VERSION ${CONFIG_CHIP_VERSION})
elseif(DEFINED ENV{TARGET_CHIP})
    # sets TARGET_CHIP_VERSION on its own
    get_chip_version($ENV{TARGET_CHIP})
endif()
message("CONFIG=${CONFIG_CHIP_VERSION} CHIP_VERSION=${TARGET_CHIP_VERSION}")


set(SRCS
    freertos_kernel/list.c
    freertos_kernel/queue.c
    freertos_kernel/tasks.c
    freertos_kernel/timers.c
    freertos_kernel/event_groups.c
    freertos_kernel/stream_buffer.c
    demos/gwt/config/gap9/FreeRTOS_util.c
    )

set(CONFIG_FAST_OSC_FREQUENCY 24576063)
set(CONFIG_FLL_MAXDCO_FREQ 900000000)
set(CONFIG_FREQUENCY_PERIPH       160000000)
set(CONFIG_FREQUENCY_FC           50000000)
set(CONFIG_FREQUENCY_CLUSTER      50000000)
set(CONFIG_FREQUENCY_SFU          50000000)
set(CONFIG_FREQUENCY_FPGA         50000000)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

set(freertos_flags "-D__USE_TCDM_MALLOC__=1" "-DPMSIS_DRIVERS=1" "-D__FC_MALLOC_NATIVE__=0" "-D__L2_MALLOC_NATIVE__=0" "-D__PMSIS_L2_MALLOC_NATIVE__=0" "-D__riscv__" "-D__GAP__" "-D__RISCV_ARCH_GAP__=1" "-D__FREERTOS__" "-DFEATURE_CLUSTER=1" "-DCONFIG_GAP9_V2" "-std=gnu99")
list(APPEND freertos_flags "-D__PLATFORM_GVSOC__")
list(APPEND freertos_flags "-D__PLATFORM__=ARCHI_PLATFORM_GVSOC")
list(APPEND freertos_flags "-DPRINTF_RTL")
list(APPEND freertos_flags "-DMAIN_APP_STACK_SIZE=2048")
list(APPEND freertos_flags  "-DCONFIG_FAST_OSC_FREQUENCY=${CONFIG_FAST_OSC_FREQUENCY}")
list(APPEND freertos_flags  "-DCONFIG_FLL_MAXDCO_FREQ=${CONFIG_FLL_MAXDCO_FREQ}")
list(APPEND freertos_flags  "-DCONFIG_FREQUENCY_PERIPH=${CONFIG_FREQUENCY_PERIPH}")
list(APPEND freertos_flags  "-DCONFIG_FREQUENCY_FC=${CONFIG_FREQUENCY_FC}")
list(APPEND freertos_flags  "-DCONFIG_FREQUENCY_CLUSTER=${CONFIG_FREQUENCY_CLUSTER}")
list(APPEND freertos_flags  "-DCONFIG_FREQUENCY_SFU=${CONFIG_FREQUENCY_SFU}")
list(APPEND freertos_flags  "-DCONFIG_FREQUENCY_FPGA=${CONFIG_FREQUENCY_FPGA}")
# Compiler flags

set(common_flags "-g" "-fmessage-length=0" "-fno-exceptions" "-ffunction-sections" "-fdata-sections" "-funsigned-char" "-fno-delete-null-pointer-checks" "-fomit-frame-pointer" "-Os" "-D__GAP__" "-D__${CHIP}__" "-D__RISCV_ARCH_GAP__=1" "-DCHIP_VERSION=${TARGET_CHIP_VERSION}" "-D__FREERTOS__" "-DFEATURE_CLUSTER=1" "-fno-jump-tables")

message("CHIP=${CHIP}")

if(${CHIP} STREQUAL "GAP8" )
    set(archi_flags -march=rv32imcxgap8 -mPE=8 -mFC=1)
elseif(${CHIP} STREQUAL "GAP9")
    set(archi_flags "-march=rv32imcxgap9" "-mint64" "-mPE=8" "-mFC=1")
endif()
set(asm_flags ${archi_flags} ${common_flags} ${warnings} "-DASSEMBLY_LANGUAGE -x assembler-with-cpp")
#string(REPLACE ";" " " linker_options "${linker_opt}")
#string(REPLACE ";" " " freertos_flags "${freertos_flags}")
#string(REPLACE ";" " " common_flags "${common_flags}")
#string(REPLACE ";" " " archi_flags "${archi_flags}")
#string(REPLACE ";" " " asm_flags "${asm_flags}")
#add_compile_options(
#    $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>
#    )
#add_compile_options(  
#    ${freertos_flags} ${common_flags} ${archi_flags}>
#  )
set(ld_script ${CMAKE_CURRENT_SOURCE_DIR}/vendors/gwt/gap9/src/device/ld/GAP9.ld)
set(linker_opt -nostartfiles -nostdlib -T${ld_script} -MD -MP -MT  ${archi_flags} -Wl,--gc-sections,-Map=$@.map,-static,-lgcc -lgcc )
#add_link_options(${linker_opt})


add_subdirectory(vendors/gwt)
add_subdirectory(demos/gwt/config/gap9)
add_subdirectory(freertos_kernel)
add_subdirectory(freertos_kernel/portable)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pmsis ${CMAKE_CURRENT_BINARY_DIR}/pmsis)



set_source_files_properties(${GCC_CRT_OBJS} PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
add_library(freertos STATIC ${ASM_SRCS} ${SRCS} )

#target_compile_options(freertos PUBLIC
#    $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>)
target_compile_options(freertos PUBLIC
    $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>
    )
target_compile_options(freertos PUBLIC
    $<$<COMPILE_LANGUAGE:C>:${freertos_flags} ${common_flags} ${archi_flags}>
    )
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
target_link_libraries(freertos PUBLIC freertos_printf)
target_link_libraries(freertos PUBLIC freertos_kernel)
target_link_libraries(freertos PUBLIC freertos_config_gap9)
target_link_libraries(freertos PUBLIC pmsis_rtos)
target_link_libraries(freertos PUBLIC freertos_pmsis_backend)
if(CONFIG_USE_BSP)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pmsis/pmsis_bsp ${CMAKE_CURRENT_BINARY_DIR}/pmsis_bsp)
    target_link_libraries(freertos PUBLIC bsp)
endif()

if(CONFIG_USE_BSP)
install(TARGETS freertos ri5cy-gap9 freertos_gap9 pmsis_implem_gap9 pmsis_freertos
     freertos_libs pmsis_api freertos_printf freertos_kernel pmsis_implem freertos_config_gap9
     pmsis_rtos freertos_pmsis_backend bsp
     EXPORT freertosTargets)
else()
install(TARGETS freertos ri5cy-gap9 freertos_gap9 pmsis_implem_gap9 pmsis_freertos
     freertos_libs pmsis_api freertos_printf freertos_kernel pmsis_implem freertos_config_gap9
     pmsis_rtos freertos_pmsis_backend
     EXPORT freertosTargets)

endif()
export(EXPORT freertosTargets
     FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/freertosTargets.cmake"
     NAMESPACE freertos::
     )
