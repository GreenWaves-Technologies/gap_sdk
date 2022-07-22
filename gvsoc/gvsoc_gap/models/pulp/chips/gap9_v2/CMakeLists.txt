if("${TARGET_CHIP}" STREQUAL "GAP9_V2")

set(CHIPS_GAP9_V2_PREFIX "pulp/chips/gap9_v2")

vp_model(NAME gap9_v2_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${CHIPS_GAP9_V2_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )

vp_files(FILES "rom.bin" "debug_rom.bin" "boot-gap9_v2"
    PREFIX ${CHIPS_GAP9_V2_PREFIX}
    )

endif()