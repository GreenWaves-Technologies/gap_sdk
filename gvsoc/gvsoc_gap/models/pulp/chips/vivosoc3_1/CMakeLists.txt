set(CHIPS_VIVOSOC3_1_PREFIX "pulp/chips/vivosoc3_1")

vp_model(NAME vivosoc3_1_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_VIVOSOC3_1_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin"
    PREFIX ${CHIPS_VIVOSOC3_1_PREFIX}
    )
