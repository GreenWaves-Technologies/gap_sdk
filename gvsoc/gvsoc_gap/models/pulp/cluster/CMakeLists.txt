set(CLUSTER_PREFIX "pulp/cluster")

vp_model(NAME l1_interleaver_impl
    PREFIX ${CLUSTER_PREFIX}
    SOURCES "l1_interleaver_impl.cpp"
    )

vp_model(NAME cluster_ctrl_v2_impl
    PREFIX ${CLUSTER_PREFIX}
    SOURCES "cluster_ctrl_v2_impl.cpp"
    )
