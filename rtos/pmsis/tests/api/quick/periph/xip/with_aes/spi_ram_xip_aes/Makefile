# User Test
#------------------------------------

APP              = test_spi_ram_xip_aes
APP_SRCS        += test_spi_ram_xip_aes.c
APP_INC         +=
APP_CFLAGS      += -Wno-shift-count-overflow
#APP_CFLAGS      += -DTRACE_OCTOSPI=1

override PMSIS_OS = freertos

# ==================
# CUSTOM Hyper Setup
# ==================

#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a spiram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_PSRAM_VIP=1

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk
