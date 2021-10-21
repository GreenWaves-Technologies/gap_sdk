set(MEMORY_PREFIX "memory")

vp_model(NAME memory_impl
    PREFIX ${MEMORY_PREFIX}
    SOURCES "memory_impl.cpp"
    )

vp_model(NAME ddr_impl
    PREFIX ${MEMORY_PREFIX}
    SOURCES "memory_impl.cpp"
    )
