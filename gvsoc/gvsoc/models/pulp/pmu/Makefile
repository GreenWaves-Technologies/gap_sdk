ifeq '$(pmu/version)' '1'
IMPLEMENTATIONS += pulp/pmu/pmu_v1_impl
COMPONENTS += pulp/pmu/pmu_v1
pulp/pmu/pmu_v1_impl_SRCS = pulp/pmu/pmu_v1_impl.cpp
endif

ifeq '$(pmu/version)' '2'
IMPLEMENTATIONS += pulp/pmu/pmu_v2_impl
COMPONENTS += pulp/pmu/pmu_v2
pulp/pmu/pmu_v2_impl_SRCS = pulp/pmu/pmu_v2_impl.cpp
endif

ifeq '$(pmu/version)' '3'
IMPLEMENTATIONS += pulp/pmu/pmu_v3_impl
COMPONENTS += pulp/pmu/pmu_v3
pulp/pmu/pmu_v3_impl_SRCS = pulp/pmu/pmu_v3_impl.cpp
endif
