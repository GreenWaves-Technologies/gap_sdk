macro(setupcrosscompile)
    # Find GAP RISCV GCC compiler
    find_program(GAP_RISCV_CC riscv32-unknown-elf-gcc)
    find_program(GAP_RISCV_CXX riscv32-unknown-elf-g++)
    find_program(GAP_RISCV_AR riscv32-unknown-elf-ar)
    find_program(GAP_RISCV_OBJDUMP riscv32-unknown-elf-objdump)
    find_program(GAP_RISCV_NM riscv32-unknown-elf-nm)
    find_program(GAP_RISCV_SIZE riscv32-unknown-elf-size)

    if(NOT GAP_RISCV_CC)
        message("Error : could not find GAP RISCV GCC toolchain !\n
        Please add it to your path (gap_riscv_toolchain)")
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
endmacro()

macro(setupsdk)
    # Find Gapy prog to exec binary produced
    find_program(GAPY gapy)
    if(NOT GAPY)
        message("Error : gapy not found !")
    endif()
endmacro()

macro(setplatform)
    if(DEFINED CONFIG_PLATFORM)
        set(GAPY_PLATFORM ${CONFIG_PLATFORM})
    else()
        set(GAPY_PLATFORM gvsoc)
        if(DEFINED ENV{platform})
            set(GAPY_PLATFORM $ENV{platform})
        else()
            set(GAPY_PLATFORM gvsoc)
        endif()
    endif()
    # set target chain (py target or old targets)
    if(DEFINED CONFIG_GAPY_TARGET)
        set(GAPY_TARGET_CMD "--target=${CONFIG_GAPY_TARGET}")
    elseif(DEFINED CONFIG_GAPY_PY_TARGET)
        set(GAPY_TARGET_CMD "--py_target=${CONFIG_GAPY_PY_TARGET}")
    elseif(DEFINED ENV{GAPY_PY_TARGET})
        set(GAPY_TARGET_CMD "--py-target=$ENV{GAPY_PY_TARGET}")
    endif()
endmacro()

macro(setupcustomtargets TARGET GAPY)
    setplatform()
    # Post processing command to dump disassembly file and size sorted file"
    set(OBJDUMP_OPT -d -h -S -t -w --show-raw-insn)
    add_custom_target(
        dump_dis DEPENDS ${TARGET}
        COMMENT "Invoking: OBJDUMP, dump disassembly in ${CMAKE_BINARY_DIR}/${TARGET}.s"
        COMMAND ${CMAKE_OBJDUMP} ${OBJDUMP_OPT} ${CMAKE_BINARY_DIR}/${TARGET} > ${CMAKE_BINARY_DIR}/${TARGET}.s
        )

    add_custom_target(
        dump_size DEPENDS ${TARGET}
        COMMENT "Invoking: SIZE NM, dump size in ${CMAKE_BINARY_DIR}/${TARGET}.size"
        COMMAND ${CMAKE_GCC_SIZE} ${SIZE_OPT} ${CMAKE_BINARY_DIR}/${TARGET} > ${CMAKE_BINARY_DIR}/${TARGET}.size
        COMMAND ${CMAKE_NM} ${NM_OPT} ${CMAKE_BINARY_DIR}/${TARGET} >> ${CMAKE_BINARY_DIR}/${TARGET}.size
        )

    add_custom_target(disdump ALL DEPENDS dump_dis dump_size)

    # Convert string to list of words with space as a separator.
    # Otherwise cmake escapes space or adds quotes in the final shell command line.
    # Here it's assumed that GAPY_RUNNER_ARGS is provided in command line as a single quoted string:
    # -DGAPY_RUNNER_ARGS="--trace=foo --trace=bar"


    separate_arguments(gapy_runner_args UNIX_COMMAND "$ENV{runner_args}")

    # Ninja and Makefile have different ways to access env var
    if( ${CMAKE_GENERATOR} STREQUAL "Ninja" )
        set(GAPY_RUNNER_ARGS $$runner_args)
    else()
        set(GAPY_RUNNER_ARGS $(runner_args))
    endif()

    # Create image
    add_custom_target(
        image DEPENDS ${TARGET}
        COMMAND ${GAPY} ${GAPY_TARGET_CMD} --platform=${GAPY_PLATFORM} --work-dir=${CMAKE_BINARY_DIR} ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --image --binary=${CMAKE_BINARY_DIR}/${TARGET} ${GAPY_RUNNER_ARGS}
        )

    # FLash image
    add_custom_target(
        flash DEPENDS image
        COMMAND ${GAPY} ${GAPY_TARGET_CMD} --platform=${GAPY_PLATFORM} --work-dir=${CMAKE_BINARY_DIR} ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --flash --binary=${CMAKE_BINARY_DIR}/${TARGET} ${GAPY_RUNNER_ARGS}
        )

    # Run exec
    add_custom_target(
        run DEPENDS flash
        COMMAND ${GAPY} ${GAPY_TARGET_CMD} --platform=${GAPY_PLATFORM} --work-dir=${CMAKE_BINARY_DIR} ${GAPY_CONFIG_ARGS} ${GAPY_ARGS} run --exec-prepare --exec --binary=${CMAKE_BINARY_DIR}/${TARGET} ${GAPY_RUNNER_ARGS}
        )
endmacro()

macro(subincludeos OS_NAME OS_PATH TARGET_NAME TARGET_PATH)
    if (CONFIG_USE_PREBUILT)
        include(${CONFIG_GAP_SDK_HOME}/${OS_PATH}/${TARGET_PATH})

        find_library(${OS_NAME} NAME ${OS_NAME})

        #add_subdirectory(${CONFIG_GAP_SDK_HOME}/rtos/freeRTOS)
        target_link_libraries(${TARGET_NAME} PUBLIC ${OS_NAME}::${OS_NAME})
    else()
        # add_subdirectory(${CONFIG_GAP_SDK_HOME}/${OS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${OS_NAME})
        target_link_libraries(${TARGET_NAME} PUBLIC ${OS_NAME})
    endif()
endmacro()

# Use includeos (instead of setupos) in targets that are part of project where an os has already been added.
# Can be invoked multiple times within a cmake project
macro(includeos TARGET_NAME)
    if(CONFIG_PMSIS_OS)
        set(OS_NAME ${CONFIG_PMSIS_OS})
    else()
        if(DEFINED ENV{PMSIS_OS})
            set(OS_NAME $ENV{PMSIS_OS})
        else()
            # by default use freertos
            set(OS_NAME "freertos")
        endif()
    endif()
    if(${OS_NAME} STREQUAL "freertos")
        # path in sdk
        set(OS_PATH "rtos/freeRTOS")
        # used for pre build freertos
        set(OS_TARGET_PATH "build/cmake/freertosTargets.cmake")
        subincludeos(${OS_NAME} ${OS_PATH} ${TARGET_NAME} ${OS_TARGET_PATH})
    endif()
endmacro()

macro(subaddos OS_NAME OS_PATH TARGET_NAME TARGET_PATH)
    if (CONFIG_USE_PREBUILT)
        # Already exists, nothing to do
    else()
        add_subdirectory(${CONFIG_GAP_SDK_HOME}/${OS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${OS_NAME})
    endif()
endmacro()

# Creates an OS target. Can be called only once in cmake project
macro(addos TARGET_NAME)
    if(CONFIG_PMSIS_OS)
        set(OS_NAME ${CONFIG_PMSIS_OS})
    else()
        if(DEFINED ENV{PMSIS_OS})
            set(OS_NAME $ENV{PMSIS_OS})
        else()
            # by default use freertos
            set(OS_NAME "freertos")
        endif()
    endif()
    if(${OS_NAME} STREQUAL "freertos")
        # path in sdk
        set(OS_PATH "rtos/freeRTOS")
        # used for pre build freertos
        set(OS_TARGET_PATH "build/cmake/freertosTargets.cmake")
        subaddos(${OS_NAME} ${OS_PATH} ${TARGET_NAME} ${OS_TARGET_PATH})
    endif()
endmacro()

macro(includebsp TARGET_NAME)
        add_subdirectory(${CONFIG_GAP_SDK_HOME}/rtos/pmsis/pmsis_bsp ${CMAKE_CURRENT_BINARY_DIR}/bsp)
        target_link_libraries(${TARGET_NAME} PRIVATE bsp)
endmacro()

macro(includetestbench TARGET_NAME)
    add_subdirectory(${CONFIG_GAP_SDK_HOME}/libs/gap_lib/testbench ${CMAKE_CURRENT_BINARY_DIR}/testbench)
    target_link_libraries(${TARGET_NAME} PUBLIC testbench)
    set(GAPY_CONFIG_ARGS ${GAPY_CONFIG_ARGS} PARENT_SCOPE)
endmacro()

macro(setupos TARGET_NAME)
    addos(${TARGET_NAME})
    includeos(${TARGET_NAME})
    setupsdk()
    setupcustomtargets(${TARGET_NAME} ${GAPY})
endmacro()

# Get chip version based on env string format
function(get_chip_version TARGET_CHIP)
    if(${TARGET_CHIP} STREQUAL "GAP8")
        set(TARGET_CHIP_VERSION "1")
    endif()
    if(${TARGET_CHIP} STREQUAL "GAP8_V2")
        set(TARGET_CHIP_VERSION "2")
    endif()
    if(${TARGET_CHIP} STREQUAL "GAP8_V3")
        set(TARGET_CHIP_VERSION "3")
    endif()
    if(${TARGET_CHIP} STREQUAL "GAP9")
        set(TARGET_CHIP_VERSION "1")
    endif()
    if(${TARGET_CHIP} STREQUAL "GAP9_V2")
        set(TARGET_CHIP_VERSION "2")
    endif()
endfunction()
