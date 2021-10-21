### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

APP              = test
APP_SRCS        += thermal_eye.c 
APP_INC	        += .
APP_CFLAGS      +=

GAP_FLAGS += -I$(TILER_INC) 

io=host


prepare_debug:
	mkdir -p offset_img
	rm -rf offset_img/*
	mkdir -p dump_out_imgs
	rm -rf dump_out_imgs/*


all:: prepare_debug

include $(RULES_DIR)/pmsis_rules.mk
