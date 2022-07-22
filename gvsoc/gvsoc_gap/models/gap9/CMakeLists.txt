add_subdirectory(cpu)

set(CLUSTER_PREFIX "gap9")

vp_model(NAME cluster
    PREFIX ${CLUSTER_PREFIX}
    SOURCES "cluster.cpp"
    )
