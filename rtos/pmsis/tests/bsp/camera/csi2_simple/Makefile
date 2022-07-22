APP = test
APP_SRCS += test.c
APP_CFLAGS += -O3 -g

TEST_TYPE ?= quick

ifeq '$(TEST_TYPE)' 'fast'
APP_CFLAGS += -DTEST_TYPE_FAST
height = 20
endif

CONFIG_GAP_LIB_IMGIO = 1


# Which camera ?
#       |--> Default : OV9281
#       |              |--> Grayscale
#       |              |--> raw format : 8 bits
#       |
#       |--> Choice  : OV5647 (define OV5647)
#                      |--> Color rgb
#                      |--> raw format : 10 bits (default) or 8 bits (define RAW8)
#                      |                                      |--> for now we go into it
#                      by removing the 2 lsb beyond the 10 bits efficients of a pixel.
#                      TODO: Find how to activate raw 8 bits configuration directly on the
#                      camera.
#
OV5647 = 1
RAW8   = 1
ifdef OV5647
	APP_CFLAGS += -DOV5647
ifdef RAW8
	APP_CFLAGS += -DRAW8
endif
endif

# Choose picture size
#        |--> Default : VGA      (both cameras)
#        |--> Choices :
#                  |--> 640*400  (both cameras) (define SIZE_640_400)
#                  |--> 1280*800 (OV9281)       (define SIZE_1280_800)
#                  |--> QVGA     (OV5647)       (define QVGA)
#
# SIZE_640_400  = 1
# SIZE_1280_800 = 1
QVGA          = 1
ifdef OV5647
	ifdef SIZE_640_400
		APP_CFLAGS += -DSIZE_640_400
	else
	ifdef QVGA
		APP_CFLAGS += -DQVGA
	endif
	endif #SIZE_640_400
else
	ifdef SIZE_640_400
		APP_CFLAGS += -DSIZE_640_400
	else
	ifdef SIZE_1280_800
		APP_CFLAGS += -DSIZE_1280_800
	endif
	endif #SIZE_640_400
endif #OV5647

# Saving picture ?
APP_CFLAGS += -DSAVE_PIC


APP_CFLAGS += -DI3C_I2C
APP_CFLAGS += -DLOGS

bayer2rgb:
	python bayer2rgb.py

include $(RULES_DIR)/pmsis_rules.mk
