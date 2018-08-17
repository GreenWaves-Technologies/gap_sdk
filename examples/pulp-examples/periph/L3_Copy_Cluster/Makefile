# Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g

# Tell the flasher, add Makefile into the file system, flash the file system and boot the test from jtag.
PLPBRIDGE_FLAGS += -f Makefile -jtag

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
