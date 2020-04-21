
#
# DRIVERS
#


# PADS
ifeq '$(CONFIG_PADS_ENABLED)' '1'
ifneq '$(padframe/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/pads/pads-v$(padframe/version).c
endif
endif


# SPIM

ifeq '$(CONFIG_SPIM_ENABLED)' '1'
ifneq '$(soc/spi_master)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/spim/spim-v0.c
endif

ifneq '$(udma/spim/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/spim/spim-v$(udma/spim/version).c drivers_deprecated/spim/spiflash-v$(udma/spim/version).c
endif
endif


# HYPER

ifeq '$(CONFIG_HYPER_ENABLED)' '1'
ifneq '$(udma/hyper/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/hyper/hyperram-v$(udma/hyper/version).c drivers_deprecated/hyper/hyperflash-v$(udma/hyper/version).c
endif
endif


# MRAM

ifeq '$(CONFIG_MRAM_ENABLED)' '1'
ifneq '$(udma/mram/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/mram/mram-v$(udma/mram/version).c
endif
endif


# UART

ifeq '$(CONFIG_UART_ENABLED)' '1'
ifneq '$(udma/uart/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/uart/uart.c
endif

ifneq '$(soc/apb_uart)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/uart/uart-v0.c
endif
endif


# I2S

ifeq '$(CONFIG_I2S_ENABLED)' '1'
ifneq '$(udma/i2s/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/i2s/i2s-v$(udma/i2s/version).c
endif
endif


# CAM

ifeq '$(CONFIG_CAM_ENABLED)' '1'
ifneq '$(udma/cpi/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/camera/himax.c drivers_deprecated/camera/camera.c
endif
endif


# I2C

ifeq '$(CONFIG_I2C_ENABLED)' '1'
ifneq '$(udma/i2c/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/i2c/i2c-v$(udma/i2c/version).c
PULP_LIB_FC_SRCS_rt += drivers_deprecated/i2c/eeprom.c
endif
endif


# RTC

ifeq '$(CONFIG_RTC_ENABLED)' '1'
PULP_FC_CFLAGS += -DRT_CONFIG_RTC_ENABLED
ifneq '$(rtc/version)' ''
ifneq '$(rtc/version)' '1'
PULP_LIB_FC_SRCS_rt += drivers_deprecated/rtc/rtc_v$(rtc/version).c
PULP_LIB_FC_ASM_SRCS_rt += drivers_deprecated/rtc/rtc_v$(rtc/version)_asm.S
else
PULP_LIB_FC_SRCS_rt += drivers_deprecated/dolphin/rtc.c
endif
endif
endif


# GPIO

ifeq '$(CONFIG_GPIO_ENABLED)' '1'
PULP_FC_CFLAGS += -DRT_CONFIG_GPIO_ENABLED
ifneq '$(gpio/version)' ''
PULP_LIB_FC_SRCS_rt += drivers_deprecated/gpio/gpio-v$(gpio/version).c
PULP_LIB_FC_ASM_SRCS_rt += kernel/riscv/gpio.S
endif

ifeq '$(pulp_chip_family)' 'usoc_v1'
PULP_LIB_FC_SRCS_rt += kernel/usoc_v1/gpio.c
endif

endif






ifeq '$(CONFIG_FLASH_FS_ENABLED)' '1'
PULP_LIB_FC_SRCS_rt += drivers_deprecated/flash.c drivers_deprecated/read_fs.c
endif