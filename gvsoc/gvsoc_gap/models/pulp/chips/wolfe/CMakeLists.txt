set(CHIPS_WOLFE_PREFIX "pulp/chips/wolfe")

vp_model(NAME wolfe_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_WOLFE_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin"
    PREFIX ${CHIPS_WOLFE_PREFIX}
    )
