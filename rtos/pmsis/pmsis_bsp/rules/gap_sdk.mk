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

TARGET_CHIP_VERSION=
ifeq ($(TARGET_CHIP), GAP8)
TARGET_CHIP_VERSION=1
else ifeq ($(TARGET_CHIP), GAP8_V2)
TARGET_CHIP_VERSION=2
else ifeq ($(TARGET_CHIP), GAP8_V3)
TARGET_CHIP_VERSION=3
else
TARGET_CHIP_VERSION=1
endif

BSP_PATH = $(CURDIR)
BSP_INC = $(CURDIR)/include
TARGET_INSTALL_DIR ?= $(CURDIR)/install
LIB_DIR ?= $(TARGET_INSTALL_DIR)/lib/gap
BUILD_DIR   ?= $(CURDIR)/build

LIB_VEGA = $(LIB_DIR)/vega/libpibsp.a
LIB_GAPUINO = $(LIB_DIR)/gapuino/libpibsp.a
LIB_AI_DECK = $(LIB_DIR)/ai_deck/libpibsp.a
LIB_GAPOC_A = $(LIB_DIR)/gapoc_a/libpibsp.a
LIB_GAPOC_A_V2 = $(LIB_DIR)/gapoc_a_revb/libpibsp.a
LIB_GAPOC_B = $(LIB_DIR)/gapoc_b/libpibsp.a
LIB_GAPOC_B_V2 = $(LIB_DIR)/gapoc_b_revb/libpibsp.a

VEGA_BUILD_DIR = $(BUILD_DIR)/bsp/vega
GAPUINO_BUILD_DIR = $(BUILD_DIR)/bsp/gapuino
AI_DECK_BUILD_DIR = $(BUILD_DIR)/bsp/ai_deck
GAPOC_A_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_a
GAPOC_A_V2_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_a_revb
GAPOC_B_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_b
GAPOC_B_V2_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_b_revb

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

INC = $(BSP_INC) $(TARGET_INSTALL_DIR)/include/io $(TARGET_INSTALL_DIR)/include
INC_PATH = $(foreach d, $(INC), -I$d)
INC_PATH += -include $(TARGET_INSTALL_DIR)/include/rt/chips/$(TARGET_NAME)/config.h

OBJECTS_VEGA = $(patsubst %.c, $(VEGA_BUILD_DIR)/%.o, $(wildcard $(VEGA_SRC)))
OBJECTS_GAPUINO = $(patsubst %.c, $(GAPUINO_BUILD_DIR)/%.o, $(wildcard $(GAPUINO_SRC)))
OBJECTS_AI_DECK = $(patsubst %.c, $(AI_DECK_BUILD_DIR)/%.o, $(wildcard $(AI_DECK_SRC)))
OBJECTS_GAPOC_A = $(patsubst %.c, $(GAPOC_A_BUILD_DIR)/%.o, $(wildcard $(GAPOC_A_SRC)))
OBJECTS_GAPOC_A_V2 = $(patsubst %.c, $(GAPOC_A_V2_BUILD_DIR)/%.o, $(wildcard $(GAPOC_A_SRC)))
OBJECTS_GAPOC_B = $(patsubst %.c, $(GAPOC_B_BUILD_DIR)/%.o, $(wildcard $(GAPOC_B_SRC)))
OBJECTS_GAPOC_B_V2 = $(patsubst %.c, $(GAPOC_B_V2_BUILD_DIR)/%.o, $(wildcard $(GAPOC_B_SRC)))

ifeq '$(TARGET_CHIP)' 'GAP9'
CFLAGS += -march=rv32imcxgap9
else
CFLAGS += -march=rv32imcxgap8
endif
CFLAGS += -std=gnu99 -mPE=8 -mFC=1 -D__riscv__ -Os -g -Werror -Wall
CFLAGS += -Wno-unused-variable -Wno-unused-function
CFLAGS += -MMD -MP -c
CFLAGS += -DCHIP_VERSION=$(TARGET_CHIP_VERSION) -D__PULP_OS__
CFLAGS += -fdata-sections -ffunction-sections

ifeq '$(TARGET_CHIP)' 'GAP9_V2'
all:
else
ifeq '$(TARGET_CHIP)' 'GAP9'
all: dir header vega_bsp
else
all: dir header gapuino_bsp ai_deck_bsp gapoc_a_bsp gapoc_a_v2_bsp gapoc_b_bsp gapoc_b_v2_bsp
endif
endif

dir:
	mkdir -p $(BUILD_DIR) $(TARGET_INSTALL_DIR) $(LIB_DIR)

header: $(INSTALL_HEADERS)

$(OBJECTS_VEGA) : $(VEGA_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_VEGA $< $(INC_PATH) -o $@

$(OBJECTS_GAPUINO) : $(GAPUINO_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_GAPUINO $< $(INC_PATH) -o $@

$(OBJECTS_AI_DECK) : $(AI_DECK_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_AI_DECK $< $(INC_PATH) -o $@

$(OBJECTS_GAPOC_A) : $(GAPOC_A_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_GAPOC_A $< $(INC_PATH) -o $@

$(OBJECTS_GAPOC_A_V2) : $(GAPOC_A_V2_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_GAPOC_A $< $(INC_PATH) -o $@

$(OBJECTS_GAPOC_B) : $(GAPOC_B_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_GAPOC_B $< $(INC_PATH) -o $@

$(OBJECTS_GAPOC_B_V2) : $(GAPOC_B_V2_BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DCONFIG_GAPOC_B $< $(INC_PATH) -o $@

vega_bsp: $(OBJECTS_VEGA)
	mkdir -p $(LIB_DIR)/vega
	$(AR) -r $(LIB_VEGA) $^

gapuino_bsp: $(OBJECTS_GAPUINO)
	mkdir -p $(LIB_DIR)/gapuino
	$(AR) -r $(LIB_GAPUINO) $^

ai_deck_bsp: $(OBJECTS_AI_DECK)
	mkdir -p $(LIB_DIR)/ai_deck
	$(AR) -r $(LIB_AI_DECK) $^

gapoc_a_bsp: $(OBJECTS_GAPOC_A)
	mkdir -p $(LIB_DIR)/gapoc_a
	$(AR) -r $(LIB_GAPOC_A) $^

gapoc_a_v2_bsp: $(OBJECTS_GAPOC_A_V2)
	mkdir -p $(LIB_DIR)/gapoc_a_revb
	$(AR) -r $(LIB_GAPOC_A_V2) $^

gapoc_b_bsp: $(OBJECTS_GAPOC_B)
	mkdir -p $(LIB_DIR)/gapoc_b
	$(AR) -r $(LIB_GAPOC_B) $^

gapoc_b_v2_bsp: $(OBJECTS_GAPOC_B_V2)
	mkdir -p $(LIB_DIR)/gapoc_b_revb
	$(AR) -r $(LIB_GAPOC_B_V2) $^

install: all

clean:
	rm -rf $(GAPUINO_BUILD_DIR) $(AI_DECK_BUILD_DIR) $(GAPOC_A_BUILD_DIR) $(GAPOC_A_V2_BUILD_DIR) $(VEGA_BUILD_DIR) $(GAPOC_B_V2_BUILD_DIR)
	rm -rf $(INSTALL_HEADERS)
	rm -rf $(LIB_GAPUINO) $(LIB_AI_DECK) $(LIB_GAPOC_A) $(LIB_GAPOC_A_V2) $(LIB_GAPOC_B) $(LIB_GAPOC_B_V2) $(LIB_VEGA)
	rm -rf $(CURDIR)/build $(CURDIR)/install
