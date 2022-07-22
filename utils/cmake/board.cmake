###############################################################################
# Syntax coloring
###############################################################################

set(BOARD_MSG_PREFIX              "[${BoldBlue}Board${ColourReset}]")

###############################################################################
# setup_board
# This macro identify the board that have been selected from Kconfig interface
# It sets BOARD_FLAGS variable that contain the chip's description (family and
# target) that needs to be provided as compiler flags with 
# target_compile_options function.
# It also set ARCHI_FLAGS variable depending on what Chip has been selected
# in Kconfig menu
###############################################################################

macro(setup_board)
    if (DEFINED CONFIG_CHIP_FAMILY_GAP8)

        set(CHIP "GAP8")
        list(APPEND ARCHI_FLAGS "-march=rv32imcxgap8" "-mPE=8" "-mFC=1")

        if(     DEFINED CONFIG_BOARD_GAPOC_A
            OR  DEFINED CONFIG_BOARD_GAPOC_B
            OR  DEFINED CONFIG_BOARD_GAPUINO)
            set(TARGET_CHIP_VERSION 1)

        elseif(     DEFINED CONFIG_BOARD_GAPOC_A_V2
                OR  DEFINED CONFIG_BOARD_GAPOC_A_SPI_V2
                OR  DEFINED CONFIG_BOARD_GAPOC_B_V2
                OR  DEFINED CONFIG_BOARD_GAPOC_B_SPI_V2
                OR  DEFINED CONFIG_BOARD_GAPUINO_V2
                OR  DEFINED CONFIG_BOARD_GAPUINO_V2_SPI)
                set(TARGET_CHIP_VERSION 2)

        elseif(     DEFINED CONFIG_BOARD_GAPUINO_V3
                OR  DEFINED CONFIG_BOARD_GAPUINO_V3_SPI)
                set(TARGET_CHIP_VERSION 3)

        else()
            message(FATAL_ERROR "Unknown GAP8 board configuration")
        endif()
    elseif(DEFINED CONFIG_CHIP_FAMILY_GAP9)

        set(CHIP "GAP9")
        list(APPEND ARCHI_FLAGS "-march=rv32imcxgap9" "-mint64" "-mPE=8" "-mFC=1")

        if(     DEFINED CONFIG_BOARD_GAP9_EVK 
            OR  DEFINED CONFIG_BOARD_GAP9_V2)

            set(TARGET_CHIP_VERSION 2)

        elseif( DEFINED CONFIG_BOARD_GAP9_5)

            set(TARGET_CHIP_VERSION 5)

        else()
            message(FATAL_ERROR "Unknown GAP9 board configuration")
        endif()
    endif()

    list(APPEND BOARD_FLAGS "-D__${CHIP}__" "-DCHIP_VERSION=${TARGET_CHIP_VERSION}")
    message(STATUS ${BOARD_MSG_PREFIX} " Chip family  : ${CHIP}")
    message(STATUS ${BOARD_MSG_PREFIX} " Chip version : ${TARGET_CHIP_VERSION}")
endmacro()