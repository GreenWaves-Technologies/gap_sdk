# Copyright 2019 GreenWaves Technologies, SAS
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


APP = cannyEdge
APP_SRCS = CannyDetect.c $(GAP_LIB_PATH)/img_io/ImgIO.c
APP_INC  = $(GAP_LIB_PATH)/include
APP_CFLAGS += -O3 -g -DFROM_FILE

APP_CFLAGS += -mno-memcpy -fno-tree-loop-distribute-patterns
APP_CFLAGS += -I$(TILER_EMU_INC) -I$(TILER_INC)
APP_LDFLAGS +=  -flto -Wl,--gc-sections

# The generated code outputs a maybe-uninitialized error which is rather difficult to suppress
# in a clean way.
APP_CFLAGS += -Wall -Werror -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef

clean::
	rm -rf img_OUT.ppm

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
