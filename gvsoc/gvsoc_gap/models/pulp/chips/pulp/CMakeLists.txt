set(CHIPS_PULP_PREFIX "pulp/chips/pulp")

vp_model(NAME pulp_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_PULP_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin"
    PREFIX ${CHIPS_PULP_PREFIX}
    )
