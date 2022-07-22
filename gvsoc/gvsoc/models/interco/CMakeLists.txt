set(INTERCO_PREFIX "interco")

add_subdirectory(mem_plug)

vp_model(NAME router_impl
    PREFIX ${INTERCO_PREFIX}
    SOURCES "router_impl.cpp"
    )

vp_model(NAME interleaver_impl
    PREFIX ${INTERCO_PREFIX}
    SOURCES "interleaver_impl.cpp"
    )

vp_model(NAME converter_impl
    PREFIX ${INTERCO_PREFIX}
    SOURCES "converter_impl.cpp"
    )

vp_model(NAME bus_watchpoint
    PREFIX ${INTERCO_PREFIX}
    SOURCES "bus_watchpoint.cpp"
    )
