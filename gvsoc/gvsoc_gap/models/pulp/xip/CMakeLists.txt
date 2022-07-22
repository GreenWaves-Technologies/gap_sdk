vp_model(NAME xip
    PREFIX "pulp/xip"
    SOURCES "xip.cpp"
    )
vp_model_link_libraries(
    NAME xip
    LIBRARY gvsoc_gap_headers
    )

vp_model_link_blocks(
    NAME xip
    BLOCK mem_plug
    )
    