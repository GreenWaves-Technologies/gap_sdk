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


PULP_APP = cannyEdge
PULP_APP_SRCS = CannyDetect.c ImgIO.c
PULP_CFLAGS += -O3 -g -DFROM_FILE

# Should be removed once openocd is the default bridge
export GAP_USE_OPENOCD=1
io=host

clean::
	rm -rf img_OUT.ppm

include $(GAP_SDK_HOME)/tools/rules/pulp_rules.mk
