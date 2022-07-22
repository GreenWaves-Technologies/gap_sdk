update_id ?= 0
blink_speed ?= 1   # Can be > 1 to slown down blinking

APP              = test
APP_SRCS        += helloworld.c
APP_INC	        +=
APP_CFLAGS      += -DUPDATE_ID=$(update_id) -DBLINK_SPEED=$(blink_speed)

# The "image" target will force the production of the flash image containing this application
do_all: all image
	cp $(BUILDDIR)/target.board.devices.flash.img ../flash.img

include $(RULES_DIR)/pmsis_rules.mk
