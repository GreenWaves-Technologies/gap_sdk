if("${CONFIG_GVSOC_SKIP_UDMA_BUILD}" EQUAL "1")
    message(NOTICE "GVSOC SKIP UDMA BUILD")
else()
if("${TARGET_CHIP}" STREQUAL "GAP8")
    message(FATAL_ERROR "GAP8 compilation is not supported by GVSOC")
elseif("${TARGET_CHIP}" STREQUAL "GAP8_V2")
    message(FATAL_ERROR "GAP8_V2 compilation is not supported by GVSOC")
elseif("${TARGET_CHIP}" STREQUAL "GAP8_V3")
    message(FATAL_ERROR "GAP8_V3 compilation is not supported by GVSOC")
elseif("${TARGET_CHIP}" STREQUAL "GAP9")
    message(FATAL_ERROR "GAP9 (vega) compilation is not supported by GVSOC")
elseif("${TARGET_CHIP}" STREQUAL "GAP9_V2")
    # Model versions
    set(UDMA_UART_VERSION  "2")
    set(UDMA_AES_VERSION   "1")
    set(UDMA_FFC_VERSION   "1")
    set(UDMA_I2C_VERSION   "4")
    set(UDMA_I2S_VERSION   "3")
    set(UDMA_HYPER_VERSION "3")
    set(UDMA_MRAM_VERSION  "2")
    set(UDMA_SFU_VERSION   "1")

    set(UDMA_SOURCES
        "udma_v4_impl.cpp"
        "dc_fifo.cpp"
        "udma_v4_addrgens.cpp"
        "udma_v4_rx_channels.cpp"
        "udma_v4_tx_channels.cpp"
        )

    vp_model(NAME "udma_model"
        OUTPUT_NAME "udma_v4_gap9_v2_impl"
        PREFIX "pulp/udma"
        SOURCES ${UDMA_SOURCES}
        )

    # FIXME ugly workaround because of some udma models
    vp_model_include_directories(
        NAME udma_model
        DIRECTORY
        "../../../../../rtos/pulp/gap_archi/include/archi/chips/gap9_v2"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DUDMA_VERSION=4
        )
else()
    message(FATAL_ERROR "${TARGET_CHIP} is not known by GVSOC")
endif()

# Needed by SFU model which is in another repository, but compiled the same way
# as the other UDMA peripherals.
vp_model_include_directories(
    NAME udma_model
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

if("${UDMA_SPIM_VERSION}" EQUAL "4")
    #target_sources(udma_model PRIVATE
    #    "spim/udma_spim_v4.cpp"
    #    )
    #target_compile_definitions(udma_model
    #    PRIVATE
    #    -DHAS_SPIM
    #    )
endif()

if("${UDMA_UART_VERSION}" EQUAL "2")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "uart/v2/udma_uart_rx.cpp"
        "uart/v2/udma_uart_tx.cpp"
        "uart/v2/udma_uart.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_UART
        )
endif()

if("${UDMA_CPI_VERSION}" EQUAL "2")
    #target_sources(udma_model PRIVATE
    #    "cpi/udma_cpi_v2.cpp"
    #    )
    #target_compile_definitions(udma_model
    #    PRIVATE
    #    -DHAS_CPI
    #    )
endif()

if("${UDMA_AES_VERSION}" EQUAL "1")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "aes/udma_aes_model_v1.cpp"
        "aes/udma_aes_v1.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_AES
        )
endif()

if(NOT ${UDMA_HAS_SFU})
    if("${UDMA_SFU_VERSION}" EQUAL "1")
        #target_sources(udma_model PRIVATE
        #    "sfu/udma_sfu_v1_empty.cpp"
        #    )
        #target_compile_definitions(udma_model
        #    PRIVATE
        #    -DHAS_EMPTY_SFU
        #    )
    endif()
endif()

if("${UDMA_FFC_VERSION}" EQUAL "1")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "ffc/udma_ffc_v1.cpp"
        "ffc/udma_ffc_v1_model.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_FFC
        )
endif()

if("${UDMA_I2S_VERSION}" EQUAL "3")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "i2s/udma_i2s_v3.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_I2S
        -DI2S_VERSION=3
        )
endif()

if("${UDMA_I2C_VERSION}" EQUAL "4")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "i2c/v4/udma_i2c.cpp"
        "i2c/v4/udma_i2c_fifos.cpp"
        "i2c/v4/udma_i2c_ucode.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_I2C
        )
    vp_model_link_libraries(
        NAME udma_model
        LIBRARY i2c_helper
        )
endif()

if("${UDMA_HYPER_VERSION}" EQUAL "3")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "hyper/udma_hyper_v3.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_HYPER
        )
endif()
endif()


if("${UDMA_MRAM_VERSION}" EQUAL "2")
    vp_model_sources(
        NAME udma_model
        SOURCES
        "mram/udma_mram_v2.cpp"
        )
    vp_model_compile_definitions(
        NAME udma_model
        DEFINITIONS
        -DHAS_MRAM
        )
endif()

