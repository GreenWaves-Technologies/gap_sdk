# Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#
PULP_APP = test
PULP_APP_FC_SRCS = test.c
PULP_CFLAGS = -O3 -g

PLPBRIDGE_FLAGS += -f hello.txt Makefile  test.c  testset.ini  version.log -jtag

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
