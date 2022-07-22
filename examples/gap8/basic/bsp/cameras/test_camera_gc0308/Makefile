# User Test
#------------------------------------
APP              = test
# App sources
APP_SRCS         = test.c
# App includes
APP_INC	         =
# Compiler flags
APP_CFLAGS       = -O2 -g
# Linker flags
APP_LDFLAGS      =

MAIN_STACK_SIZE=4096
#APP_CFLAGS += -DTRACE_CPI

## This is working only with QVGA
#APP_CFLAGS += -DOUT_TO_DISPLAY 

#Choose between RGB (565) and Grayscale
#APP_CFLAGS += -DGRAYSCALE

#Choose between VGA and QVGA 
#APP_CFLAGS += -DVGA


#On Gapoc C the gapmod has QSPI ram and flash so this flag should be activated
#APP_CFLAGS += -DQSPI

# Include GAP_LIB/IMGIO
CONFIG_GAP_LIB_IMGIO = 1

override config_args += --config-opt=camera/image-stream=$(CURDIR)/imgTest0.pgm
override config_args += --config-opt=runner/peripherals=true

clean::
	rm -rf *.ppm

include $(RULES_DIR)/pmsis_rules.mk
