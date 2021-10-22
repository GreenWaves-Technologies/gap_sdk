set(CHIPS_GAP_PREFIX "pulp/chips/gap")

vp_model(NAME gap_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_GAP_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin"
    PREFIX ${CHIPS_GAP_PREFIX}
    )
