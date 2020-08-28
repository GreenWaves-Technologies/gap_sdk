APP = test
APP_SRCS += test.c

NB_FRAME ?= -1

APP_CFLAGS += -O3 -g -DNB_FRAME=$(NB_FRAME) -Werror

# Give GVSOC device configuration to activate LCD model as it is there by default
RUNNER_CONFIG = $(CURDIR)/config.ini

# Give camera input image to GVSOC model. It is also possible to specify img%d.pgm to give a set of images
override config_args += --config-opt=camera/image-stream=$(CURDIR)/Lena.pgm

include $(RULES_DIR)/pmsis_rules.mk
