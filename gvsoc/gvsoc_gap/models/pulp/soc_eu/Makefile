ifeq '$(soc_eu/version)' '1'
IMPLEMENTATIONS += pulp/soc_eu/soc_eu_v1_impl
pulp/soc_eu/soc_eu_v1_impl_SRCS = pulp/soc_eu/soc_eu_v1_impl.cpp
endif

ifeq '$(soc_eu/version)' '2'
IMPLEMENTATIONS += pulp/soc_eu/soc_eu_v2_impl
pulp/soc_eu/soc_eu_v2_impl_SRCS = pulp/soc_eu/soc_eu_v2_impl.cpp
endif

ifeq '$(TARGET_CHIP)' 'GAP9_V2'
IMPLEMENTATIONS += pulp/soc_eu/soc_eu_v3_impl
pulp/soc_eu/soc_eu_v3_impl_SRCS = pulp/soc_eu/soc_eu_v3_impl.cpp
endif