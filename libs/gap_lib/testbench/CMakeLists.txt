cmake_minimum_required(VERSION 3.16)

if (NOT CONFIG_TESTBENCH_UART_ID)
    set(CONFIG_TESTBENCH_UART_ID 1)
endif()

if (NOT CONFIG_TESTBENCH_UART_BAUDRATE)
    set(CONFIG_TESTBENCH_UART_BAUDRATE 20000000)
endif()

set(testbench_flags
    "-DCONFIG_TESTBENCH_UART_ID=${CONFIG_TESTBENCH_UART_ID}"
    "-DCONFIG_TESTBENCH_UART_BAUDRATE=${CONFIG_TESTBENCH_UART_BAUDRATE}")

set(C_SRCS
    testbench.c
    testlib.c
    )

add_library(testbench OBJECT ${C_SRCS})

target_link_libraries(testbench PRIVATE freertos)

target_include_directories(testbench PUBLIC .)

target_compile_options(testbench PUBLIC $<$<COMPILE_LANGUAGE:C>:${testbench_flags}>)

set(GWT_PMSIS_BSP ${CONFIG_GAP_SDK_HOME}/rtos/pmsis/pmsis_bsp)
target_include_directories(testbench PRIVATE ${GWT_PMSIS_BSP}/include)

#===================================================================================================
# gapy config arguments
#===================================================================================================
if (NOT CONFIG_BOARD_PATH)
    set(CONFIG_BOARD_PATH target)
endif()

if (NOT GAPY_CONFIG_ARGS)
    set(GAPY_CONFIG_ARGS "")
endif()

list(APPEND GAPY_CONFIG_ARGS --config-opt=${CONFIG_BOARD_PATH}/addon_testbench_enabled=true)
list(APPEND GAPY_CONFIG_ARGS --config-opt=**/runner/gvsoc_dpi/enabled=true)
list(APPEND GAPY_CONFIG_ARGS --config-opt=**/testbench/testbench/uart_id=${CONFIG_TESTBENCH_UART_ID})
list(APPEND GAPY_CONFIG_ARGS --config-opt=**/testbench/testbench/uart_baudrate=${CONFIG_TESTBENCH_UART_BAUDRATE})

set(GAPY_CONFIG_ARGS ${GAPY_CONFIG_ARGS} PARENT_SCOPE)
