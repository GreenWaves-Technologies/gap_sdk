#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#$(info #### PMSIS BSP PATH : $(PMSIS_BSP_DIR))

include $(PMSIS_BSP_DIR)/src.mk

ifeq ($(BOARD_NAME), gapuino)
PMSIS_BSP_SRC = $(GAPUINO_SRC)
else ifeq ($(BOARD_NAME), gapoc_a)
PMSIS_BSP_SRC = $(GAPOC_A_SRC)
else ifeq ($(BOARD_NAME), gapoc_a_revb)
PMSIS_BSP_SRC = $(GAPOC_A_SRC)
else ifeq ($(BOARD_NAME), gapoc_b)
PMSIS_BSP_SRC = $(GAPOC_B_SRC)
else ifeq ($(BOARD_NAME), gapoc_b_revb)
PMSIS_BSP_SRC = $(GAPOC_B_SRC)
else ifeq ($(BOARD_NAME), vega)
PMSIS_BSP_SRC = $(GAP9_SRC)
else ifeq ($(BOARD_NAME), gap9_v2)
PMSIS_BSP_SRC = $(GAP9_SRC)
endif

EXCLUDE_FROM_SRCS= transport/transport.c transport/nina_w10/nina_w10.c

PMSIS_BSP_SRCS := $(filter-out $(EXCLUDE_FROM_SRCS), $(PMSIS_BSP_SRC))
PMSIS_BSP_SRCS := $(foreach f, $(PMSIS_BSP_SRCS), $(PMSIS_BSP_DIR)/$f)

PMSIS_BSP_INC = $(PMSIS_BSP_DIR)/include

#$(info #### Board : $(BOARD_NAME))
#$(info #### Sources : $(PMSIS_BSP_SRCS))
#$(info #### Include : $(PMSIS_BSP_INC))
