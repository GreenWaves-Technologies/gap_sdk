set(FLL_PREFIX "pulp/fll")

vp_model(NAME fll_v1_impl
    PREFIX ${FLL_PREFIX}
    SOURCES "fll_v1_impl.cpp"
    )

vp_model(NAME fll_v2_impl
    PREFIX ${FLL_PREFIX}
    SOURCES "fll_v2_impl.cpp"
    )

vp_model(NAME fll_ctrl_impl
    PREFIX ${FLL_PREFIX}
    SOURCES "fll_ctrl_impl.cpp"
    )
