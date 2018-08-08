# Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
PULP_APP = test
PULP_APP_FC_SRCS = test.c ImgIO.c
PULP_CFLAGS = -O3

#PLPBRIDGE_FLAGS += -s camera@test.py

imgOut:
	mkdir $@

all:: imgOut

clean::
	rm -rf imgOut

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
