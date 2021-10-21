set(CHIPS_PULPISSIMO_PREFIX "pulp/chips/pulpissimo")

vp_model(NAME pulpissimo_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_PULPISSIMO_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin"
    PREFIX ${CHIPS_PULPISSIMO_PREFIX}
    )
