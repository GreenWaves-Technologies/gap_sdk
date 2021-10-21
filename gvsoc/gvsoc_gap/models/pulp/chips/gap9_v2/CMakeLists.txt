set(CHIPS_GAP9_V2_PREFIX "pulp/chips/gap9_v2")

vp_model(NAME gap9_v2_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_GAP9_V2_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin"
    PREFIX ${CHIPS_GAP9_V2_PREFIX}
    )
