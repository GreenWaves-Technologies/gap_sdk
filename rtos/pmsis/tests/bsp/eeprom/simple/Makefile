APP = test
APP_SRCS = test.c
APP_CFLAGS += -O3 -g

CONFIG_24XX1025 = 1

config_args += --config-opt=rtl/args=+ENABLE_VIP_EEPROM_0=1

ifdef CONFIG_NO_FAST_OSC
override config_args += --config-opt=**/rtl/args=+DISABLE_FAST_CLOCK=1
endif

ifdef CONFIG_OPEN_LOOP

override config_args += --config-opt=**/rtl/args=+DISABLE_FAST_CLOCK=1

# Deactivate efuse generation to get empty efuses and execute in open-loop
override config_args += --config-opt=**/runner/efuses/enabled=false

# Bootmode is specified on pads since efuses are empty
override config_args += --config-opt=**/runner/boot/jtag_mode=2

override config_args += --config-opt=**/rtl/args=+ENABLE_HYPER0_CS0_HYPERFLASH_VIP=1
override config_args += --config-opt=**/rtl/args=+VIP_MODE=CUSTOM

endif

include $(RULES_DIR)/pmsis_rules.mk
