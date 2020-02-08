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


# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

GCC_CHECK = $(shell which $(CC))
ifeq ($(GCC_CHECK),)
$(error Please make sure that you have installed the toolchain for GAP)
endif

MODULE_PATH = $(CURDIR)
MODULE_INC = $(CURDIR)/include
TARGET_INSTALL_DIR ?= $(CURDIR)/install
LIB_DIR ?= $(TARGET_INSTALL_DIR)/lib/gap
BUILD_DIR   ?= $(CURDIR)/build

LIB_BUILD_DIR = $(BUILD_DIR)/gap_lib/$(BOARD_NAME)

LIB_PATH = $(LIB_BUILD_DIR)/libgaptools.a
INSTALL_LIB_PATH = $(LIB_DIR)/$(BOARD_NAME)/libgaptools.a

$(info #### Building in $(BUILD_DIR))
$(info #### Installing to $(TARGET_INSTALL_DIR))

#
## HEADER RULES
#

define declareInstallFile

$(TARGET_INSTALL_DIR)/$(1): $(1)
	        install -D $(1) $$@

INSTALL_HEADERS += $(TARGET_INSTALL_DIR)/$(1)

endef

INSTALL_FILES  = $(shell find include -name *.h)
$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

INC = $(MODULE_INC) $(TARGET_INSTALL_DIR)/include/io $(TARGET_INSTALL_DIR)/include
INC_PATH = $(foreach d, $(INC), -I$d)
INC_PATH += -include $(TARGET_INSTALL_DIR)/include/rt/chips/$(TARGET_NAME)/config.h

OBJECTS = $(patsubst %.c, $(LIB_BUILD_DIR)/%.o, $(wildcard $(SRC)))

ifeq '$(TARGET_CHIP)' 'GAP9'
CFLAGS += -march=rv32imcxgap9
else
CFLAGS += -march=rv32imcxgap8
endif
CFLAGS += -std=gnu99 -mPE=8 -mFC=1 -D__riscv__ -O2 -g -Werror -Wall
CFLAGS += -Wno-unused-variable -Wno-unused-function
CFLAGS += -MMD -MP -c

all_target: dir header $(INSTALL_LIB_PATH)

dir:
	mkdir -p $(BUILD_DIR) $(TARGET_INSTALL_DIR) $(LIB_DIR)

header_target: $(INSTALL_HEADERS)

$(OBJECTS) : $(LIB_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_$(BOARD_NAME) $< $(INC_PATH) -o $@

$(LIB_PATH): $(OBJECTS)
	mkdir -p $(LIB_DIR)/$(BOARD_NAME)
	$(AR) -r $(LIB_PATH) $^

$(INSTALL_LIB_PATH): $(LIB_PATH)
	install -D $^ $@



clean_target:
	rm -rf $(LIB_BUILD_DIR)
	rm -rf $(INSTALL_HEADERS)
	rm -rf $(LIB_PATH)
	rm -rf $(CURDIR)/build $(CURDIR)/install