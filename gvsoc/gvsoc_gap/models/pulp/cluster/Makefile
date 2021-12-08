ifneq '$(cluster/version)' ''

ifeq '$(cluster_ctrl/version)' '2'
IMPLEMENTATIONS += pulp/cluster/cluster_ctrl_v2_impl
pulp/cluster/cluster_ctrl_v2_impl_SRCS = pulp/cluster/cluster_ctrl_v2_impl.cpp
endif

IMPLEMENTATIONS += pulp/cluster/l1_interleaver_impl
pulp/cluster/l1_interleaver_impl_SRCS = pulp/cluster/l1_interleaver_impl.cpp

endif
