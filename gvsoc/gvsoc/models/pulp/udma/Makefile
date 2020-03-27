pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DUDMA_VERSION=$(udma/version)

ifeq '$(udma/spim/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_SPIM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/spim/udma_spim_v2.cpp
endif

ifeq '$(udma/spim/version)' '3'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_SPIM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/spim/udma_spim_v3.cpp
endif

ifeq '$(udma/spim/version)' '4'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_SPIM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/spim/udma_spim_v4.cpp
endif

ifeq '$(udma/cpi/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_CPI
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/cpi/udma_cpi_v1.cpp
endif

ifeq '$(udma/hyper/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_HYPER
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/hyper/udma_hyper_v1.cpp
endif

ifeq '$(udma/hyper/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_HYPER
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/hyper/udma_hyper_v2.cpp
endif

ifeq '$(udma/hyper/version)' '3'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_HYPER
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/hyper/udma_hyper_v3.cpp
endif

ifeq '$(udma/i2c/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_I2C
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/i2c/udma_i2c_v2.cpp
endif



ifneq '$(udma/i2s/version)' ''
UDMA_CFLAGS += -DI2S_VERSION=$(udma/i2s/version)
endif

ifeq '$(udma/i2s/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_I2S
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/i2s/udma_i2s_v1.cpp
endif

ifeq '$(udma/i2s/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_I2S
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/i2s/udma_i2s_v2.cpp
endif

ifeq '$(udma/i2s/version)' '3'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_I2S
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/i2s/udma_i2s_v3.cpp
endif



ifeq '$(udma/mram/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_MRAM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/mram/udma_mram_v1.cpp
endif

ifeq '$(udma/tcdm/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_TCDM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/tcdm/udma_tcdm_v1.cpp
endif

ifeq '$(udma/version)' '2'
IMPLEMENTATIONS += pulp/udma/udma_v2_$(pulp_chip_family)_impl
pulp/udma/udma_v2_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_v2_impl.cpp
pulp/udma/udma_v2_$(pulp_chip_family)_impl_SRCS += pulp/udma/uart/udma_uart_v1.cpp
endif

ifeq '$(udma/version)' '3'
IMPLEMENTATIONS += pulp/udma/udma_v3_$(pulp_chip_family)_impl
pulp/udma/udma_v3_$(pulp_chip_family)_impl_CFLAGS += $(UDMA_CFLAGS)
pulp/udma/udma_v3_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_v3_impl.cpp
pulp/udma/udma_v3_$(pulp_chip_family)_impl_SRCS += pulp/udma/io/udma_io.cpp
pulp/udma/udma_v3_$(pulp_chip_family)_impl_SRCS += pulp/udma/uart/udma_uart_v1.cpp
endif

ifeq '$(udma/version)' '4'
IMPLEMENTATIONS += pulp/udma/udma_v4_$(pulp_chip_family)_impl
pulp/udma/udma_v4_$(pulp_chip_family)_impl_CFLAGS += -I$(INSTALL_DIR)/include/archi/chips/$(pulp_chip_family)
pulp/udma/udma_v4_$(pulp_chip_family)_impl_CFLAGS += $(UDMA_CFLAGS)
pulp/udma/udma_v4_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_v4_impl.cpp
endif

ifeq '$(udma/uart/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_UART
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/uart/udma_uart_v2.cpp
endif
