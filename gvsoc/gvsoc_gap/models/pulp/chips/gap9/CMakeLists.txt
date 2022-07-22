set(CHIPS_GAP9_PREFIX "pulp/chips/gap9")

vp_model(NAME gap9_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_GAP9_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin"
    PREFIX ${CHIPS_GAP9_PREFIX}
    )
