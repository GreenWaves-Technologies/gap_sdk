set(OPRECOMPKW_PREFIX "pulp/chips/oprecompkw")

vp_model(NAME oprecompkw_job_fifo_impl
    OUTPUT_NAME "job_fifo_impl"
    PREFIX ${OPRECOMPKW_PREFIX}
    SOURCES "job_fifo_impl.cpp"
    )

vp_model(NAME oprecompkw_apb_soc_impl
    OUTPUT_NAME "apb_soc_impl"
    PREFIX ${OPRECOMPKW_PREFIX}
    SOURCES "apb_soc_impl.cpp"
    )
