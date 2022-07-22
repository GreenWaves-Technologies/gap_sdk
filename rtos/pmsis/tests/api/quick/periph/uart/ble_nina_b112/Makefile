APP = ble_nina_b112
APP_SRCS += ble_test.c
APP_CFLAGS +=
#APP_CFLAGS += -DTRACE_UART=1

USE_PMSIS_BSP=1
CONFIG_BLE_NINA_B112=1

runner_args +=--trace=nina_b112 --trace-level=info

override config_args += --config-opt=**/runner/gvsoc_dpi/enabled=true

ifeq '$(FLOW_CONTROL)' '1'
override config_args += --config-opt=**/nina_b112/nina/rts/enabled=true
APP_CFLAGS+= -DTEST_FLOW_CONTROL=1
endif

ifeq '$(BOARD_NAME)' 'gap9_evk'
APP_CFLAGS += -DCONFIG_NINA_B112_UART_ID=1
APP_CFLAGS += -DGPIOA2_NINA_RST=40
APP_CFLAGS += -DGPIOA21_NINA17=41
APP_CFLAGS += -DGPIO_NINA_PWRON=42
APP_CFLAGS += -DGPIO_NINA17_DSR=43
endif

ifeq '$(BOARD_NAME)' 'gap9_v2'
APP_CFLAGS += -DCONFIG_NINA_B112_UART_ID=1
APP_CFLAGS += -DGPIOA2_NINA_RST=40
APP_CFLAGS += -DGPIOA21_NINA17=41
APP_CFLAGS += -DGPIO_NINA_PWRON=42
APP_CFLAGS += -DGPIO_NINA17_DSR=43
endif

include $(RULES_DIR)/pmsis_rules.mk
