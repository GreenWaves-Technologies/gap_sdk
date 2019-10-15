PP = test
PULP_APP_FC_SRCS = test.c ImgIO.c
PULP_APP_HOST_SRCS = test.c
PULP_CFLAGS = -O3 -g
PLPBRIDGE_FLAGS += -fileIO

clean::
	rm -rf Pedestrian_OUT.ppm

include $(PULP_SDK_HOME)/tools/rules/pulp_rules.mk
