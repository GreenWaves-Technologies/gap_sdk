# Hello World
project(hello_world C ASM)

cmake_minimum_required(VERSION 3.0)

# APP sources
set(APP_SRCS helloworld.c)
# APP cflags
set(APP_CFLAGS "")


# Find GAP RISCV GCC compiler
find_program(GAP_RISCV_CC riscv32-unknown-elf-gcc)
find_program(GAP_RISCV_CXX riscv32-unknown-elf-g++)
find_program(GAP_RISCV_AR riscv32-unknown-elf-ar)
find_program(GAP_RISCV_OBJDUMP riscv32-unknown-elf-objdump)
find_program(GAP_RISCV_NM riscv32-unknown-elf-nm)
find_program(GAP_RISCV_SIZE riscv32-unknown-elf-size)
if(NOT GAP_RISCV_CC)
  message("Error : could not find GAP RISCV GCC toolchain ! Source sourceme.sh in SDK !")
else()
  set(GAP_RISCV_ASM ${GAP_RISCV_CC})
  # message("GAP RISCV GCC=" ${GAP_RISCV_CC})
endif()

# Specify the cross compiler
set(CMAKE_C_COMPILER ${GAP_RISCV_CC})
set(CMAKE_CXX_COMPILER ${GAP_RISCV_CXX})
set(CMAKE_ASM_COMPILER ${GAP_RISCV_ASM})
set(CMAKE_OBJDUMP ${GAP_RISCV_OBJDUMP})
set(CMAKE_NM ${GAP_RISCV_NM})
# Specific
set(CMAKE_GCC_SIZE ${GAP_RISCV_SIZE})

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME FreeRTOS)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

if (CMAKE_CROSSCOMPILING)
  message("Crosscompiling for " ${CMAKE_SYSTEM_NAME} " on " ${CMAKE_SYSTEM_PROCESSOR} " using compiler  found at " ${CMAKE_C_COMPILER})
endif()


# Find Gapy prog to exec binary produced
find_program(GAPY gapy)
if(NOT GAPY)
  message("Error : gapy not found !")
endif()
#message("gapy=" ${GAPY})

set(GAPY_PLATFORM gvsoc)
if(DEFINED $ENV{platform})
  set(GAPY_PLATFORM $ENV{platform})
else()
  set(GAPY_PLATFORM gvsoc)
endif()
#message("Exec on " ${GAPY_PLATFORM})

# -> BUILD_DIR := build
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/BUILD)

set(TARGET helloworld)

# For debug, enable verbose Makefile : -DMK_VERBOSE=ON
option(MK_VERBOSE "Makefile verbose" OFF)
set(CMAKE_VERBOSE_MAKEFILE ${MK_VERBOSE})
if(MK_VERBOSE STREQUAL ON)
  message("Enable Makefile verbose")
endif()

#set(FREERTOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(FREERTOS_SOURCE_DIR $ENV{FREERTOS_PATH}/freertos_kernel)
set(FREERTOS_CONFIG_DIR $ENV{FREERTOS_PATH}/demos/gwt/gap9/common/config_files)
set(PORT_DIR            ${FREERTOS_SOURCE_DIR}/portable/GCC/RI5CY-GAP9)
set(GWT_DIR             $ENV{FREERTOS_PATH}/vendors/gwt)
set(GWT_TARGET          ${GWT_DIR})
set(GWT_LIBS            ${GWT_TARGET}/libs)
set(GWT_DEVICE          ${GWT_TARGET}/gap9/src/device)
set(GWT_DRIVER          ${GWT_TARGET}/gap9/src/driver)
set(GWT_DEVICE_INC      ${GWT_TARGET}/gap9/include/device)
set(GWT_DRIVER_INC      ${GWT_TARGET}/gap9/include/driver)
set(GWT_PMSIS           ${GWT_TARGET}/pmsis)
set(GWT_PMSIS_BACKEND   ${GWT_PMSIS}/backend)
set(GWT_PMSIS_RTOS      ${GWT_PMSIS}/rtos)
set(GWT_PMSIS_IMPLEM    ${GWT_TARGET}/gap9/pmsis)
set(GWT_PMSIS_API       $ENV{GAP_SDK_HOME}/rtos/pmsis/pmsis_api)
set(GWT_PMSIS_BSP       $ENV{GAP_SDK_HOME}/rtos/pmsis/pmsis_bsp)

# Include dir
include_directories(
  .
  ${FREERTOS_SOURCE_DIR}/include
  ${PORT_DIR}
  ${FREERTOS_CONFIG_DIR}
  ${GWT_TARGET}
  ${GWT_DEVICE_INC}
  ${GWT_DRIVER_INC}
  ${GWT_LIBS}/include
  ${GWT_LIBS}/printf
  ${GWT_PMSIS}/include
  ${GWT_PMSIS_BACKEND}
  ${GWT_PMSIS_RTOS}
  ${GWT_PMSIS_RTOS}/include
  ${GWT_PMSIS_IMPLEM}
  ${GWT_PMSIS_IMPLEM}/include
  ${GWT_PMSIS_IMPLEM}/include/cores/TARGET_RISCV_32
  ${GWT_PMSIS_IMPLEM}/include/pmsis/implem
  ${GWT_PMSIS_IMPLEM}/include/pmsis/targets
  ${GWT_PMSIS_API}/include
  ${GWT_PMSIS_BSP}/include
  )

# Sources
FILE(GLOB FREERTOS_SOURCES_ASM
  ${PORT_DIR}/port_asm.S
  ${GWT_DEVICE}/gap9_iet.S
  ${GWT_DEVICE}/startup_gap9.S
  )

FILE(GLOB FREERTOS_SOURCES
  ${FREERTOS_SOURCE_DIR}/list.c
  ${FREERTOS_SOURCE_DIR}/queue.c
  ${FREERTOS_SOURCE_DIR}/tasks.c
  ${FREERTOS_SOURCE_DIR}/timers.c
  ${FREERTOS_SOURCE_DIR}/event_groups.c
  ${FREERTOS_SOURCE_DIR}/stream_buffer.c
  ${FREERTOS_CONFIG_DIR}/FreeRTOS_util.c
  ${PORT_DIR}/port.c
  ${GWT_DEVICE}/gap9_it.c
  ${GWT_DEVICE}/system_gap9.c
  ${GWT_DRIVER}/gap_io.c
  ${GWT_LIBS}/printf/printf.c
  ${GWT_LIBS}/src/errno.c
  ${GWT_LIBS}/src/stdlib.c
  ${GWT_LIBS}/src/string.c
  )

LIST(APPEND FREERTOS_SOURCES
  ${GWT_PMSIS_BACKEND}/pmsis_backend_native_task_api.c
  ${GWT_PMSIS_RTOS}/event_kernel/event_kernel.c
  ${GWT_PMSIS_RTOS}/malloc/cl_l1_malloc.c
  ${GWT_PMSIS_RTOS}/malloc/fc_l1_malloc.c
  ${GWT_PMSIS_RTOS}/malloc/l2_malloc.c
  ${GWT_PMSIS_RTOS}/malloc/malloc_external.c
  ${GWT_PMSIS_RTOS}/malloc/malloc_internal.c
  ${GWT_PMSIS_RTOS}/malloc/pi_malloc.c
  ${GWT_PMSIS_RTOS}/mem_slab/mem_slab.c
  ${GWT_PMSIS_RTOS}/os/device.c
  ${GWT_PMSIS_RTOS}/os/pmsis_task.c
  ${GWT_PMSIS_RTOS}/pi_log.c
  )

LIST(APPEND FREERTOS_SOURCES_ASM
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/cluster_synchronisation/cluster_core.S
  ${GWT_PMSIS_IMPLEM}/util/asm_util.S
  )

LIST(APPEND FREERTOS_SOURCES
  # Cluster sources.
  ${GWT_PMSIS_IMPLEM}/cluster/malloc/cl_malloc.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/cluster_synchronisation/cl_to_fc_delegate.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/cluster_synchronisation/fc_to_cl_delegate.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/cluster_team/cl_team.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/delegate/uart/uart_cl_internal.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/decompressor/cl_dma_decompressor.c
  ${GWT_PMSIS_IMPLEM}/cluster/drivers/decompressor/cl_dma_decompressor_internal.c

  # Drivers sources.
  ${GWT_PMSIS_IMPLEM}/drivers/dmacpy/dmacpy.c
  ${GWT_PMSIS_IMPLEM}/drivers/fc_event/fc_event.c
  ${GWT_PMSIS_IMPLEM}/drivers/fll/fll.c
  ${GWT_PMSIS_IMPLEM}/drivers/gpio/gpio.c
  ${GWT_PMSIS_IMPLEM}/drivers/pad/pad.c
  ${GWT_PMSIS_IMPLEM}/drivers/perf/perf_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/pmu/pmu.c
  ${GWT_PMSIS_IMPLEM}/drivers/pwm/pwm.c
  ${GWT_PMSIS_IMPLEM}/drivers/pwm/pwm_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/rtc/rtc.c
  ${GWT_PMSIS_IMPLEM}/drivers/rtc/rtc_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/timer/timer.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/hyperbus/hyperbus.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/hyperbus/hyperbus_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/aes/aes.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/aes/aes_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/aes_dual_core/aes_dual_core_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/asrc/asrc.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/asrc/asrc_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/cpi/cpi.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/cpi/cpi_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/ffc/ffc.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2s/i2s.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2s/i2s_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2c/i2c.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2c/i2c_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2c/i2c_slave.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/i2c/i2c_slave_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/octospi/octospi.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/octospi/octospi_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/spim/spim.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/uart/uart.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/uart/uart_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/udma_core/udma_core.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/udma_core/udma_timeout.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/udma_datamove/udma_datamove.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/udma_fifo/udma_fifo.c
  ${GWT_PMSIS_IMPLEM}/drivers/quiddikey/quiddikey_internal.c
  ${GWT_PMSIS_IMPLEM}/drivers/udma/udma_core/udma_timestamp.c
  ${GWT_PMSIS_IMPLEM}/drivers/xip/xip_internal.c
  )

LIST(APPEND FREERTOS_SOURCES
  # BSP sources
  ${GWT_PMSIS_BSP}/flash/flash.c
  ${GWT_PMSIS_BSP}/partition/partition.c
  ${GWT_PMSIS_BSP}/partition/flash_partition.c
  ${GWT_PMSIS_BSP}/crc/md5.c
  ${GWT_PMSIS_BSP}/flash/hyperflash/hyperflash.c
#  ${GWT_PMSIS_BSP}/flash/spiflash/spiflash.c
  ${GWT_PMSIS_BSP}/flash/spiflash/atxp032.c
  ${GWT_PMSIS_BSP}/ram/ram.c
  ${GWT_PMSIS_BSP}/ram/alloc_extern.c
  ${GWT_PMSIS_BSP}/ram/hyperram/hyperram.c
#  ${GWT_PMSIS_BSP}/ram/spiram/spiram.c
  ${GWT_PMSIS_BSP}/ram/spiram/aps25xxxn.c
  ${GWT_PMSIS_BSP}/fs/fs.c
  ${GWT_PMSIS_BSP}/fs/host_fs/semihost.c
  ${GWT_PMSIS_BSP}/fs/host_fs/host_fs.c
  ${GWT_PMSIS_BSP}/fs/read_fs/read_fs.c
  ${GWT_PMSIS_BSP}/fs/lfs/lfs.c
  ${GWT_PMSIS_BSP}/fs/lfs/lfs_util.c
  ${GWT_PMSIS_BSP}/fs/lfs/pi_lfs.c
  ${GWT_PMSIS_BSP}/ota/ota.c
  ${GWT_PMSIS_BSP}/ota/ota_utility.c
  ${GWT_PMSIS_BSP}/ota/updater.c
  ${GWT_PMSIS_BSP}/bootloader/bootloader_utility.c
  ${GWT_PMSIS_BSP}/eeprom/24XX1025.c
  ${GWT_PMSIS_BSP}/eeprom/virtual_eeprom.c
  ${GWT_PMSIS_BSP}/bsp/gap9_v2.c
  ${GWT_PMSIS_BSP}/camera/camera.c
  ${GWT_PMSIS_BSP}/camera/himax/himax.c
  ${GWT_PMSIS_BSP}/flash/mram/mram-v2.c
  ${GWT_PMSIS_BSP}/ble/ble.c
  ${GWT_PMSIS_BSP}/ble/nina_b112/nina_b112.c
  ${GWT_PMSIS_BSP}/ble/nina_b112/nina_b112_old.c
  )

# CRT obj.
set(
  GCC_CRT_OBJS
  $ENV{GAP_RISCV_GCC_TOOLCHAIN}/lib/gcc/riscv32-unknown-elf/7.1.1/crtbegin.o
  $ENV{GAP_RISCV_GCC_TOOLCHAIN}/lib/gcc/riscv32-unknown-elf/7.1.1/crti.o
  $ENV{GAP_RISCV_GCC_TOOLCHAIN}/lib/gcc/riscv32-unknown-elf/7.1.1/crtn.o
  $ENV{GAP_RISCV_GCC_TOOLCHAIN}/lib/gcc/riscv32-unknown-elf/7.1.1/crtend.o
  )


# Linker script
set(ld_script ${GWT_DEVICE}/ld/GAP9.ld)

# Objdump options
#set(OBJDUMP_OPT -D -l -f -g -z)
set(OBJDUMP_OPT -d -h -S -t -w --show-raw-insn)

# Nm options
set(NM_OPT -a -A -l -S --size-sort --special-syms)

# Size options
set(SIZE_OPT -B -x --common)


# Flags
set(archi_flags -march=rv32imcxgap9 -mint64 -mPE=8 -mFC=1)

# Linker flags
set(linker_flags -nostartfiles -nostdlib)
# Strip options
set(strip_opt -Wl,--gc-sections -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${TARGET}.map -Wl,-static)
set(lib_gcc -lgcc)

set(linker_opt ${archi_flags} ${linker_flags} ${strip_opt} ${lib_gcc} -T${ld_script} -MMD -MP)
string(REPLACE ";" " " linker_options "${linker_opt}")

set(freertos_flags "-D__USE_TCDM_MALLOC__=1" "-DPMSIS_DRIVERS=1" "-D__FC_MALLOC_NATIVE__=0" "-D__L2_MALLOC_NATIVE__=0" "-D__PMSIS_L2_MALLOC_NATIVE__=0" "-D__riscv__" "-D__GAP__" "-D__GAP9__" "-D__RISCV_ARCH_GAP__=1" "-DCHIP_VERSION=2" "-D__FREERTOS__" "-DFEATURE_CLUSTER=1" "-DCONFIG_GAP9_V2")

list(APPEND freertos_flags "-DPI_LOG_DEFAULT_LEVEL=PI_LOG_TRACE")

set(common_flags "-g" "-fmessage-length=0" "-fno-exceptions" "-ffunction-sections" "-fdata-sections" "-funsigned-char" "-fno-delete-null-pointer-checks" "-fomit-frame-pointer" "-Os")

# Warnings
set(warnings "-Wall" "-Wextra" "-Wno-unused-parameter" "-Wno-unused-function" "-Wno-unused-variable" "-Wno-unused-but-set-variable" "-Wno-missing-field-initializers" "-Wno-format" "-Wimplicit-fallthrough=0")

# Stack size
list(APPEND freertos_flags "-DMAIN_APP_STACK_SIZE=2048")

# GVSOC
list(APPEND freertos_flags "-D__PLATFORM_GVSOC__")
list(APPEND freertos_flags "-D__PLATFORM__=ARCHI_PLATFORM_GVSOC")
list(APPEND freertos_flags "-DPRINTF_RTL")

set(asm_flags "${archi_flags}" "${common_flags}" "${warnings}" "-DASSEMBLY_LANGUAGE")
set(c_flags "-std=gnu99" "${archi_flags}" "${common_flags}" "${warnings}" "${freertos_flags}" "${APP_CFLAGS}")


# Remove cflags set(-rdynamic was set when linking)
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

# Debug
#message("Project: ${PROJECT_NAME}")
#message("APP Sources: ${APP_SRCS}")
#message("Sources: ${FREERTOS_SOURCES}")
#message("Sources ASM: ${FREERTOS_SOURCES_ASM}")


# Create executable with demo code files
set_source_files_properties(${GCC_CRT_OBJS} PROPERTIES EXTERNAL_STATIC TRUE GENERATED TRUE)

add_executable(${TARGET} ${GCC_CRT_OBJS} ${FREERTOS_SOURCES_ASM} ${FREERTOS_SOURCES} ${APP_SRCS})

# Compiler flags
target_compile_options(${TARGET} PUBLIC
  $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp ${asm_flags}>
  $<$<COMPILE_LANGUAGE:C>:${c_flags} ${APP_CFLAGS}>
  )

target_link_libraries(${TARGET} ${linker_options})


# Post processing command to dump disassembly file and size sorted file
add_custom_target(
  dump_dis DEPENDS ${TARGET}
  COMMENT "Invoking: OBJDUMP, dump disassembly in ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.s"
  COMMAND ${CMAKE_OBJDUMP} ${OBJDUMP_OPT} ${EXECUTABLE_OUTPUT_PATH}/${TARGET} > ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.s
  )

add_custom_target(
  dump_size DEPENDS ${TARGET}
  COMMENT "Invoking: SIZE NM, dump size in ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.size"
  COMMAND ${CMAKE_GCC_SIZE} ${SIZE_OPT} ${EXECUTABLE_OUTPUT_PATH}/${TARGET} > ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.size
  COMMAND ${CMAKE_NM} ${NM_OPT} ${EXECUTABLE_OUTPUT_PATH}/${TARGET} >> ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.size
  )

add_custom_target(disdump ALL DEPENDS dump_dis dump_size)

# Create image
add_custom_target(
  image DEPENDS ${TARGET}
  COMMAND ${GAPY} --target=$ENV{GAPY_TARGET} --platform=${GAPY_PLATFORM} --work-dir=${EXECUTABLE_OUTPUT_PATH}/gapy ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --image --binary=${EXECUTABLE_OUTPUT_PATH}/${TARGET} ${GAPY_RUNNER_ARGS}
  )

# FLash image
add_custom_target(
  flash DEPENDS image
  COMMAND ${GAPY} --target=$ENV{GAPY_TARGET} --platform=${GAPY_PLATFORM} --work-dir=${EXECUTABLE_OUTPUT_PATH}/gapy ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --flash --binary=${EXECUTABLE_OUTPUT_PATH}/${TARGET} ${GAPY_RUNNER_ARGS}
  )

# Run exec
add_custom_target(
  run DEPENDS flash
  COMMAND ${GAPY} --target=$ENV{GAPY_TARGET} --platform=${GAPY_PLATFORM} --work-dir=${EXECUTABLE_OUTPUT_PATH}/gapy ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --exec-prepare --exec --binary=${EXECUTABLE_OUTPUT_PATH}/${TARGET} ${GAPY_RUNNER_ARGS}
  )
