# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.



#### Model List #####
MODEL_ID?=0


ifeq ($(MODEL_ID), 0)
	MODEL_PREFIX?=mobilenet_v1_1_0_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 1)
	MODEL_PREFIX?=mobilenet_v1_1_0_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 2)
	MODEL_PREFIX?=mobilenet_v1_1_0_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 3)
	MODEL_PREFIX?=mobilenet_v1_1_0_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 4)
	MODEL_PREFIX?=mobilenet_v1_0_75_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
	MODEL_L2_MEMORY?=325000
endif

ifeq ($(MODEL_ID), 5)
	MODEL_PREFIX?=mobilenet_v1_0_75_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 6)
	MODEL_PREFIX?=mobilenet_v1_0_75_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 7)
	MODEL_PREFIX?=mobilenet_v1_0_75_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 8)
	MODEL_PREFIX?=mobilenet_v1_0_5_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 9)
	MODEL_PREFIX?=mobilenet_v1_0_5_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
	MODEL_L2_MEMORY?=325000
endif

ifeq ($(MODEL_ID), 10)
	MODEL_PREFIX?=mobilenet_v1_0_5_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 11)
	MODEL_PREFIX?=mobilenet_v1_0_5_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 12)
	MODEL_PREFIX?=mobilenet_v1_0_25_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 13)
	MODEL_PREFIX?=mobilenet_v1_0_25_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 14)
	MODEL_PREFIX?=mobilenet_v1_0_25_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 15)
	MODEL_PREFIX?=mobilenet_v1_0_25_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv1
endif

ifeq ($(MODEL_ID), 16)
	MODEL_PREFIX?=mobilenet_v2_1_4_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
	MODEL_L2_MEMORY?=350000
endif

ifeq ($(MODEL_ID), 17)
	MODEL_PREFIX?=mobilenet_v2_1_0_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
	MODEL_L2_MEMORY?=275000 # The code size is too big in this model
endif

ifeq ($(MODEL_ID), 18)
	MODEL_PREFIX?=mobilenet_v2_1_0_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 19)
	MODEL_PREFIX?=mobilenet_v2_1_0_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 20)
	MODEL_PREFIX?=mobilenet_v2_1_0_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 21)
	MODEL_PREFIX?=mobilenet_v2_1_0_96_quant
	AT_INPUT_WIDTH?=96
	AT_INPUT_HEIGHT?=96
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 22)
	MODEL_PREFIX?=mobilenet_v2_0_75_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 23)
	MODEL_PREFIX?=mobilenet_v2_0_75_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 24)
	MODEL_PREFIX?=mobilenet_v2_0_75_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 25)
	MODEL_PREFIX?=mobilenet_v2_0_75_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 26)
	MODEL_PREFIX?=mobilenet_v2_0_75_96_quant
	AT_INPUT_WIDTH?=96
	AT_INPUT_HEIGHT?=96
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 27)
	MODEL_PREFIX?=mobilenet_v2_0_5_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 28)
	MODEL_PREFIX?=mobilenet_v2_0_5_192_quant
	AT_INPUT_WIDTH?=192
	AT_INPUT_HEIGHT?=192
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 29)
	MODEL_PREFIX?=mobilenet_v2_0_5_160_quant
	AT_INPUT_WIDTH?=160
	AT_INPUT_HEIGHT?=160
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 30)
	MODEL_PREFIX?=mobilenet_v2_0_5_128_quant
	AT_INPUT_WIDTH?=128
	AT_INPUT_HEIGHT?=128
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 31)
	MODEL_PREFIX?=mobilenet_v2_0_5_96_quant
	AT_INPUT_WIDTH?=96
	AT_INPUT_HEIGHT?=96
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_mbv2
endif

ifeq ($(MODEL_ID), 32)
	MODEL_PREFIX?=mobilenet_v3_large_minimalistic_1_0_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script
endif

ifeq ($(MODEL_ID), 33)
	MODEL_PREFIX?=mobilenet_v3_small_1_0_224_quant
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script
endif

ifeq ($(MODEL_ID), 34)
	MODEL_PREFIX?=squeezenet
	AT_INPUT_WIDTH?=224
	AT_INPUT_HEIGHT?=224
	AT_INPUT_COLORS?=3
	NNTOOL_SCRIPT?=models/nntool_scripts/nntool_script_with_quantization
	QUANT_FLAG = 
endif

## AT GENERATED NAMES
AT_CONSTRUCT = $(MODEL_PREFIX)CNN_Construct
AT_DESTRUCT = $(MODEL_PREFIX)CNN_Destruct
AT_CNN = $(MODEL_PREFIX)CNN
AT_L3_ADDR = $(MODEL_PREFIX)_L3_Flash
