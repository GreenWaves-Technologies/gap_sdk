cmake_minimum_required(VERSION 3.10)
project(gvsoc-dpi)

set(COMMON_SRCS
    "src/qspim.cpp"
    "src/gpio.cpp"
    "src/jtag.cpp"
    "src/ctrl.cpp"
    "src/uart.cpp"
    "src/cpi.cpp"
    "src/i2s.cpp"
    "src/i2c.cpp"
    "src/telnet_proxy.cpp"
    )

set(DPI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/sv/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/ext/nosv/")

function(dpi_model)
    cmake_parse_arguments(
        DPI_MODEL
        ""
        "NAME"
        "SOURCES;INCLUDES"
        ${ARGN}
        )
    #message(STATUS "dpi_model: name=\"${DPI_MODEL_NAME}\", srcs=\"${DPI_MODEL_SOURCES}\", incs=\"${DPI_MODEL_INCLUDES}\"")

    # TODO verify arguments

    add_library(${DPI_MODEL_NAME} MODULE ${DPI_MODEL_SOURCES})
    target_include_directories(${DPI_MODEL_NAME} PRIVATE ${DPI_INCLUDE_DIRS})
    target_link_libraries(${DPI_MODEL_NAME} PRIVATE json-tools)
    set_target_properties(${DPI_MODEL_NAME} PROPERTIES PREFIX "")
    target_compile_options(${DPI_MODEL_NAME} PRIVATE "-DUSE_DPI")

    install(TARGETS ${DPI_MODEL_NAME}
        LIBRARY DESTINATION  "lib"
        ARCHIVE DESTINATION  "lib"
        RUNTIME DESTINATION  "bin"
        INCLUDES DESTINATION "include"
        )
endfunction()

add_subdirectory(models)

# libpulpdpi
set(PULPDPI_SRCS "src/dpi.cpp" ${COMMON_SRCS})
dpi_model(NAME libpulpdpi
    SOURCES ${PULPDPI_SRCS}
    )

# libpulpperiph
set(PULPPERIPH_SRCS "src/models.cpp" ${COMMON_SRCS})
dpi_model(NAME libpulpperiph
    SOURCES ${PULPPERIPH_SRCS}
    )
