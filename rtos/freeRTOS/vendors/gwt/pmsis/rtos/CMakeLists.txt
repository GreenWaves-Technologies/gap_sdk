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

LIST(APPEND PMSIS_RTOS_REQUIRED_SRCS    malloc/fc_l1_malloc.c
                                        malloc/l2_malloc.c
                                        malloc/malloc_internal.c
                                        malloc/pi_malloc.c
                                        os/device.c
                                        os/pmsis_task.c	
                                        event_kernel/event_kernel.c
                                        pi_log.c)   

if( DEFINED CONFIG_PMSIS_RTOS_CL_L1_MALLOC)
    LIST(APPEND PMSIS_RTOS_OPTIONAL_SRCS malloc/cl_l1_malloc.c)
endif()

if( DEFINED CONFIG_PMSIS_RTOS_MALLOC_EXTERNAL)
    LIST(APPEND PMSIS_RTOS_OPTIONAL_SRCS malloc/malloc_external.c)
endif()

if( DEFINED CONFIG_PMSIS_RTOS_MEM_SLAB)
    LIST(APPEND PMSIS_RTOS_OPTIONAL_SRCS mem_slab/mem_slab.c )
endif()

add_library(pmsis_rtos STATIC ${PMSIS_RTOS_REQUIRED_SRCS} ${PMSIS_RTOS_OPTIONAL_SRCS})

target_link_libraries(pmsis_rtos PRIVATE pmsis_freertos)
target_link_libraries(pmsis_rtos PRIVATE freertos)
target_link_libraries(pmsis_rtos PRIVATE pmsis_api)
target_link_libraries(pmsis_rtos PRIVATE freertos_config_gap9)
target_link_libraries(pmsis_rtos PRIVATE ri5cy-gap9)

target_include_directories(pmsis_rtos PUBLIC "include")
target_include_directories(pmsis_rtos PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
