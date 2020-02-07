ifeq '$(pulp_chip)' 'oprecompkw_sa'
use_oprecompkw = 1
endif
ifeq '$(pulp_chip)' 'oprecompkw_sfloat_sa'
use_oprecompkw = 1
endif

ifeq '$(use_oprecompkw)' '1'
IMPLEMENTATIONS += pulp/chips/oprecompkw_sa/apb_soc_impl
pulp/chips/oprecompkw_sa/apb_soc_impl_SRCS = pulp/chips/oprecompkw_sa/apb_soc_impl.cpp
endif
