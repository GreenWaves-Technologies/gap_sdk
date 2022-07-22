APP = test
APP_SRCS += test.c
APP_CFLAGS += -Os -g
APP_LDFLAGS += -Os -g

override config_args += --config-opt=**/runner/jtag_mode=disable

CONFIG_SLOW_OSC=1

FLASH_TYPE ?= HYPER_FLASH


ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'

APP_CFLAGS += -DUSE_HYPERFLASH

else

ifeq '$(FLASH_TYPE)' 'MRAM'

APP_CFLAGS += -DUSE_MRAM

else

APP_CFLAGS += -DUSE_SPIFLASH
CONFIG_ATXP032 = 1

endif

endif

# Flash GPIO pulse
override config_args += --config-opt=**/runner/efuses/content/padfun4=0x10000000
override config_args += --config-opt=**/runner/efuses/content/info_4/flash_gpio_pulse_gen=1
override config_args += --config-opt=**/runner/efuses/content/info_4/flash_gpio_pulse_wait=1
override config_args += --config-opt=**/runner/efuses/content/info_4/flash_gpio_pulse_pol=1
override config_args += --config-opt=**/runner/efuses/content/info_4/flash_gpio_pulse_id=78
override config_args += --config-opt=**/runner/efuses/content/flash_gpio_pulse_wait=15

ifeq '$(FLASH_TYPE)' 'SPIFLASH'
# Flash wakeup
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_wakeup=1
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_wakeup_wait=1
override config_args += --config-opt=**/runner/efuses/content/flash_wakeup_wait=45

else
ifeq '$(FLASH_TYPE)' 'HYPER_FLASH'
# Flash wakeup
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_wakeup=1
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_wakeup_wait=1
override config_args += --config-opt=**/runner/efuses/content/flash_wakeup_wait=450

# Flash reset
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_reset=1
override config_args += --config-opt=**/runner/efuses/content/info_3/flash_reset_wait=1
override config_args += --config-opt=**/runner/efuses/content/flash_reset_wait=150

endif
endif


export CONFIG_TESTBENCH=1


include $(RULES_DIR)/pmsis_rules.mk