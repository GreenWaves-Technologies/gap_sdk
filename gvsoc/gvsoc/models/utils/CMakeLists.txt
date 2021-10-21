set(UTILS_PREFIX "utils")

vp_model(NAME loader_impl
    PREFIX ${UTILS_PREFIX}
    SOURCES "loader_impl.cpp"
    )

if(NOT DEFINED ENV{GVSOC_SKIP_DPI})
    message(STATUS "TODO dpi_wrapper_impl")
    #vp_model(NAME dpi_wrapper_impl
    #    PREFIX ${UTILS_PREFIX}
    #    SOURCES "dpi_wrapper_impl.cpp"
    #    )
endif()

vp_model(NAME injector_impl
    PREFIX ${UTILS_PREFIX}
    SOURCES "injector_impl.cpp"
    )
vp_model_link_libraries(NAME injector_impl LIBRARY gvsoc_launcher_lib)

vp_model(NAME composite_impl
    PREFIX ${UTILS_PREFIX}
    SOURCES "composite_impl.cpp"
    )

vp_model(NAME clock_impl
    PREFIX ${UTILS_PREFIX}
    SOURCES "clock_impl.cpp"
    )

vp_model(NAME dpi_chip_wrapper
    PREFIX ${UTILS_PREFIX}
    SOURCES "dpi_chip_wrapper.cpp"
    )
