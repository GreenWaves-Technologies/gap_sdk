include ../../bsp_src.mk

APP = test
APP_SRCS 		= test.c
APP_SRCS        += $(GAPOC_C_SRC)
APP_INC         += ../../bsp $(PMSIS_BSP_PATH)/include
APP_CFLAGS      += $(GAPOC_C_BSP_FLAGS)
APP_LDFLAGS      =

ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'
APP_CFLAGS += -DUSE_HYPERFLASH
else
APP_CFLAGS += -DUSE_SPIFLASH
endif

FLASH_TYPE ?= SPI_FLASH
APP_CFLAGS += -O3 -g -DFLASH_TYPE=$(FLASH_TYPE)

CUSTOM_BSP = 1

include $(RULES_DIR)/pmsis_rules.mk
