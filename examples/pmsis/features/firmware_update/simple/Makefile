APP              = test
APP_SRCS        += updater.c
APP_INC	        +=
APP_CFLAGS      += -DFLASH_IMAGE=\"$(CURDIR)/flash.img\"

ID ?= 1

# Here we compile the new firmware. This will also produce the flash image and copy it here
all::
	make -C app do_all blink_speed=$(ID)

clean::
	make -C app clean

include $(RULES_DIR)/pmsis_rules.mk
