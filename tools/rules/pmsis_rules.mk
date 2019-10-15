BOARD_NAME ?= gapuino
PMSIS_OS ?= pulpos

ifeq ($(BOARD_NAME), gapoc_a)
COMMON_CFLAGS          += -DCONFIG_GAPOC_A
PLPBRIDGE_FLAGS        += -ftdi
else ifeq ($(BOARD_NAME), gapuino)
COMMON_CFLAGS          += -DCONFIG_GAPUINO
endif

ifdef RUNNER_CONFIG
override runner_args += --config-user=$(RUNNER_CONFIG)
endif

ifdef runner_args
export GVSOC_OPTIONS=$(runner_args)
endif

ifeq '$(PMSIS_OS)' 'freertos'

APP_SRC                  = $(APP_SRCS)
APP_INC_PATH            += $(APP_INC)
FREERTOS_FLAGS          += $(APP_CFLAGS) $(COMMON_CFLAGS) -D__USE_TCDM_MALLOC__=1 -DPMSIS_DRIVERS=1

# Select PMSIS drivers
USE_PMSIS_DRIVERS        = true

ifndef NO_PMSIS_BSP
PMSIS_BSP_DIR           += $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp
ifeq ($(BOARD_NAME), gapoc_a)
APP_SRC                 += $(PMSIS_BSP_DIR)/bsp/gapoc_a.c
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/camera/himax/*"
else ifeq ($(BOARD_NAME), gapuino)
APP_SRC                 += $(PMSIS_BSP_DIR)/bsp/gapuino.c
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/camera/mt9v034/*"
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/ble/*"
endif

# Exclude
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/bsp/*"
#NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/display/*"
#NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/flash/*"
#NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/fs/*"
#NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/ram/*"
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/transport/*"
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/examples/*"
NOT_IN                  += -not -path "$(PMSIS_BSP_DIR)/tests/*"

# Includes
INCLUDES                += -I$(PMSIS_BSP_DIR)/ -I$(PMSIS_BSP_DIR)/include
INCLUDES                += $(foreach f, $(shell find $(PMSIS_BSP_DIR) -iname "*.h" $(NOT_IN) -exec dirname {} \; | uniq), -I$f)

# Sources
#LIBS                   += -L$(TARGET_INSTALL_DIR)/lib/gap/$(BOARD_NAME) -lpibsp
APP_SRC                 += $(shell find $(PMSIS_BSP_DIR) -iname "*.c" $(NOT_IN))
endif

include $(GAP_SDK_HOME)/tools/rules/freeRTOS_rules.mk

else

ifeq '$(PMSIS_OS)' 'mbed'

TEST_C          = $(SRCS)
MBED_FLAGS     += -DMBED_CONF_RTOS_PRESENT=1

include $(GAP_SDK_HOME)/tools/rules/mbed_rules.mk

else

ifeq '$(PMSIS_OS)' 'zephyr'

include $(GAP_SDK_HOME)/tools/rules/zephyr_rules.mk

else

PULP_APP = $(APP)
PULP_APP_FC_SRCS = $(APP_SRCS)
PULP_APP_INC_PATH    += $(APP_INC)

PULP_CFLAGS = $(APP_CFLAGS) $(COMMON_CFLAGS)
ifdef USE_PMSIS_TOOLS
PULP_LDFLAGS += -lpitools
endif

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk

endif

endif

endif
