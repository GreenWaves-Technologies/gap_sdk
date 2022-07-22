APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

ifeq ($(USE_QK),1)
APP_CFLAGS += -DUSE_QK=1
endif


ifeq ($(QK_LOCK),1)
APP_CFLAGS += -DQK_LOCK=1
endif

ifeq ($(USE_CRC),1)
APP_CFLAGS += -DUSE_CRC=1
endif

ifeq ($(KEY_LOCK),1)
APP_CFLAGS += -DKEY_LOCK=1
endif

override PMSIS_OS=freertos

ifeq ($(USE_ITF1),1)
APP_CFLAGS += -DUSE_ITF1=1
#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code/first boot)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
#set Hyper1_CS1 (code/reboot)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS1_HYPERFLASH_VIP=1
endif

ifeq ($(USE_XIP),1)
APP_CFLAGS += -DUSE_XIP=1
#set VIP CUSTOM MODE
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM
#set Hyper0_CS1 (code/first boot)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS1_HYPERFLASH_VIP=1
#set Hyper0_CS0 to be a hyperram
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERRAM_VIP=1
#set Hyper1_CS1 (code/reboot)
override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER1_CS1_HYPERFLASH_VIP=1
endif

include $(RULES_DIR)/pmsis_rules.mk
