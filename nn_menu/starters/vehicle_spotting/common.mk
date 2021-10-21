# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

ifeq ($(RGB), 1)
	NNTOOL_SCRIPT=model/nntool_script_rgb
	MODEL_PREFIX=mobv2_035_rgb
	AT_INPUT_COLORS=3
else
	NNTOOL_SCRIPT=model/nntool_script_bw
	MODEL_PREFIX=mobv2_1_bw
	AT_INPUT_COLORS=1
endif

AT_INPUT_WIDTH=224
AT_INPUT_HEIGHT=224
