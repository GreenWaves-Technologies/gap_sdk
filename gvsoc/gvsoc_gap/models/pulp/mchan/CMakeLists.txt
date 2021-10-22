set(MCHAN_PREFIX "pulp/mchan")

vp_model(NAME mchan_v6_impl
    PREFIX ${MCHAN_PREFIX}
    SOURCES "mchan_v6_impl.cpp"
    )

vp_model(NAME mchan_v7_impl
    PREFIX ${MCHAN_PREFIX}
    SOURCES "mchan_v7_impl.cpp"
    )
