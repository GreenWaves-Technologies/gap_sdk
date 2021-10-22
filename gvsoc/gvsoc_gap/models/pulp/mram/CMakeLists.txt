vp_model(NAME mram_v1_impl
    PREFIX "pulp/mram"
    SOURCES "mram_impl.cpp"
    )
vp_model_link_libraries(
    NAME mram_v1_impl
    LIBRARY gvsoc_gap_headers
    )
