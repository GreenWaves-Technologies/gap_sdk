APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g


override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM


CONFIG_BOOT_DEVICE ?= hyperflash
FLASH_ITF ?= 0
FLASH_CS ?= 0

override config_args += --config-opt=**/runner/efuses/content/info_3/flash_cs_setup=1
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_itf_setup=1
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_cs=$(FLASH_CS)
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_itf=$(FLASH_ITF)

ifeq '$(CONFIG_BOOT_DEVICE)' 'spiflash'
ifeq '$(FLASH_ITF)' '0'
ifeq '$(FLASH_CS)' '0'
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_ATXP032_VIP=1
else
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_ATXP032_VIP=1
endif
else
ifeq '$(FLASH_CS)' '0'
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_ATXP032_VIP=1
else
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS1_ATXP032_VIP=1
endif
endif
endif

ifeq '$(CONFIG_BOOT_DEVICE)' 'hyperflash'
ifeq '$(FLASH_ITF)' '0'
ifeq '$(FLASH_CS)' '0'
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERFLASH_VIP=1
else
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
endif
else
ifeq '$(FLASH_CS)' '0'
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS0_HYPERFLASH_VIP=1
else
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS1_HYPERFLASH_VIP=1
endif
endif
endif



include $(RULES_DIR)/pmsis_rules.mk
