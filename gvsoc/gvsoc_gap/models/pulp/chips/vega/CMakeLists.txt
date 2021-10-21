set(CHIPS_VEGA_PREFIX "pulp/chips/vega")

vp_model(NAME vega_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_VEGA_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin"
    PREFIX ${CHIPS_VEGA_PREFIX})
