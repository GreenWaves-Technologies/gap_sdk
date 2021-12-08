set(VP_PREFIX "vp")

vp_model(NAME clock_domain_impl
    PREFIX ${VP_PREFIX}
    SOURCES "clock_domain_impl.cpp"
    )

vp_model(NAME time_domain_impl
   PREFIX ${VP_PREFIX}
    SOURCES "time_engine.cpp"
    )

vp_model(NAME trace_domain_impl
   PREFIX ${VP_PREFIX}
    SOURCES "trace_domain_impl.cpp"
    )
