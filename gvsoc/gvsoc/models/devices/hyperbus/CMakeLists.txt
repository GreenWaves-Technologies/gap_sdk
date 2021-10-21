set(HYPER_PREFIX "devices/hyperbus")
vp_model(NAME hyperram_impl
    PREFIX ${HYPER_PREFIX}
    SOURCES "hyperram_impl.cpp")

vp_model(NAME hyperflash_impl
    PREFIX ${HYPER_PREFIX}
    SOURCES "hyperflash_impl.cpp")
