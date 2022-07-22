APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

NB_PE ?= 8

FILE_NAME = flash_file.bin
FILE = files/$(FILE_NAME)

APP_CFLAGS += -DFILE_PATH="$(FILE_NAME)"

APP_ARCH_CFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1
APP_ARCH_LDFLAGS ?=  -march=rv32imcxgap9 -mPE=$(NB_PE) -mFC=1

APP_CFLAGS += -DNB_PE=$(NB_PE)

FLASH_TYPE ?= HYPERFLASH

READFS_FILES = $(FILE)

ifeq '$(FLASH_TYPE)' 'HYPERFLASH'

APP_CFLAGS += -DUSE_HYPERFLASH

else

ifeq '$(FLASH_TYPE)' 'MRAM'

APP_CFLAGS += -DUSE_MRAM
READFS_FLASH = target/chip/soc/mram
CONFIG_MRAM = 1

else

APP_CFLAGS += -DUSE_SPIFLASH
CONFIG_ATXP032 = 1
READFS_FLASH = target/board/devices/spiflash

endif

endif

override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM


# Deactivate efuse generation to get empty efuses
override config_args += --config-opt=**/runner/efuses/enabled=false

ifeq '$(FLASH_TYPE)' 'SPIFLASH'
# SPIFLASH is not available on pads, just set efuse bootmode to force spiflash
override config_args += --config-opt=**/runner/efuses/content/info_1/bootmode=2
override config_args += --config-opt=**/runner/boot/jtag_mode=0
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_ATXP032_VIP=1
else
# Otherwise bootmode is specified on pads since efuses are empty
ifeq '$(FLASH_TYPE)' 'HYPERFLASH'
override config_args += --config-opt=**/runner/boot/jtag_mode=2
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERFLASH_VIP=1
else
override config_args += --config-opt=**/runner/boot/jtag_mode=3
endif
endif

include $(RULES_DIR)/pmsis_rules.mk