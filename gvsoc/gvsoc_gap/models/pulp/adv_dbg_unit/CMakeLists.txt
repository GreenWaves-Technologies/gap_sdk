set(ADV_DBG_UNIT_PREFIX "pulp/adv_dbg_unit")

vp_model(NAME adv_dbg_unit_impl
    PREFIX ${ADV_DBG_UNIT_PREFIX}
    SOURCES "adv_dbg_unit_impl.cpp"
    )

vp_model(NAME riscv_dtm_impl
    PREFIX ${ADV_DBG_UNIT_PREFIX}
    SOURCES "riscv_dtm_impl.cpp"
    )
