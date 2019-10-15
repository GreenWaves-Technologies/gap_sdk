ifneq '$(cluster/version)' ''

ifeq '$(cluster_ctrl/version)' '2'
IMPLEMENTATIONS += pulp/cluster/cluster_ctrl_v2_impl
COMPONENTS += pulp/cluster/cluster_ctrl_v2
pulp/cluster/cluster_ctrl_v2_impl_SRCS = pulp/cluster/cluster_ctrl_v2_impl.cpp
endif

IMPLEMENTATIONS += pulp/cluster/l1_interleaver_impl
COMPONENTS += pulp/cluster/l1_interleaver
pulp/cluster/l1_interleaver_impl_SRCS = pulp/cluster/l1_interleaver_impl.cpp

IMPLEMENTATIONS += pulp/cluster/cluster_impl
COMPONENTS += pulp/cluster/cluster
pulp/cluster/cluster_impl_SRCS = pulp/cluster/cluster_impl.cpp

endif
