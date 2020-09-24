ifeq '$(pulp_chip)' 'oprecompkw'
inc_job_fifo=1
endif

ifeq '$(pulp_chip)' 'oprecompkw_sa'
inc_job_fifo=1
endif

ifeq '$(pulp_chip)' 'oprecompkw_sfloat'
inc_job_fifo=1
endif

ifeq '$(pulp_chip)' 'oprecompkw_sfloat_sa'
inc_job_fifo=1
endif

ifeq '$(inc_job_fifo)' '1'
IMPLEMENTATIONS += pulp/chips/oprecompkw/job_fifo_impl
pulp/chips/oprecompkw/job_fifo_impl_SRCS = pulp/chips/oprecompkw/job_fifo_impl.cpp
endif

ifeq '$(pulp_chip)' 'oprecompkw'
IMPLEMENTATIONS += pulp/chips/oprecompkw/apb_soc_impl
pulp/chips/oprecompkw/apb_soc_impl_SRCS = pulp/chips/oprecompkw/apb_soc_impl.cpp
endif
