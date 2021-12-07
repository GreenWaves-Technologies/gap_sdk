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

# Driver sources
FILE(GLOB SRCS
    event_kernel/event_kernel.c
    malloc/cl_l1_malloc.c
    malloc/fc_l1_malloc.c
    malloc/l2_malloc.c
    malloc/malloc_external.c
    malloc/malloc_internal.c
    malloc/pi_malloc.c
    mem_slab/mem_slab.c
    os/device.c
    os/pmsis_task.c
    pi_log.c
    )


FILE(GLOB ASM_SRCS
    )

add_library(pmsis_rtos STATIC ${ASM_SRCS} ${SRCS})


target_link_libraries(pmsis_rtos PRIVATE pmsis_freertos)
target_link_libraries(pmsis_rtos PRIVATE freertos)
target_link_libraries(pmsis_rtos PRIVATE pmsis_api)
target_link_libraries(pmsis_rtos PRIVATE freertos_config_gap9)
target_link_libraries(pmsis_rtos PRIVATE ri5cy-gap9)

target_include_directories(pmsis_rtos PUBLIC "include")
target_include_directories(pmsis_rtos PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
