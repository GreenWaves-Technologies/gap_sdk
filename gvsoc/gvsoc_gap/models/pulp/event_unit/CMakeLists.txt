set(EVENT_UNIT_PREFIX "pulp/event_unit")

vp_model(NAME eu_v3_impl
    PREFIX ${EVENT_UNIT_PREFIX}
    SOURCES "eu_v3_impl.cpp"
    )

vp_model(NAME eu_v4_impl
    PREFIX ${EVENT_UNIT_PREFIX}
    SOURCES "eu_v4_impl.cpp"
    )
