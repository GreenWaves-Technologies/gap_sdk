# Copyright (c) 2017 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

GCC_CHECK = $(shell which $(CC))
ifeq ($(GCC_CHECK),)
$(error Please make sure that you have installed the toolchain for GAP)
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
LIB_GAPOC_B = $(LIB_DIR)/gapoc_b/libpibsp.a

VEGA_BUILD_DIR = $(BUILD_DIR)/bsp/vega
GAPUINO_BUILD_DIR = $(BUILD_DIR)/bsp/gapuino
AI_DECK_BUILD_DIR = $(BUILD_DIR)/bsp/ai_deck
GAPOC_A_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_a
GAPOC_B_BUILD_DIR = $(BUILD_DIR)/bsp/gapoc_b

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
OBJECTS_GAPOC_B = $(patsubst %.c, $(GAPOC_B_BUILD_DIR)/%.o, $(wildcard $(GAPOC_B_SRC)))

ifeq '$(TARGET_CHIP)' 'GAP9'
CFLAGS += -march=rv32imcxgap9
else
CFLAGS += -march=rv32imcxgap8
endif
CFLAGS += -std=gnu99 -mPE=8 -mFC=1 -D__riscv__ -Os -g -Werror -Wall
CFLAGS += -Wno-unused-variable -Wno-unused-function
CFLAGS += -MMD -MP -c

ifeq '$(TARGET_CHIP)' 'GAP9'
all: dir header vega_bsp
else
all: dir header gapuino_bsp ai_deck_bsp gapoc_a_bsp gapoc_b_bsp
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

$(OBJECTS_GAPOC_B) : $(GAPOC_B_BUILD_DIR)/%.o : %.c
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

gapoc_b_bsp: $(OBJECTS_GAPOC_B)
	mkdir -p $(LIB_DIR)/gapoc_b
	$(AR) -r $(LIB_GAPOC_B) $^

install: all

clean:
	rm -rf $(GAPUINO_BUILD_DIR) $(AI_DECK_BUILD_DIR) $(GAPOC_A_BUILD_DIR) $(VEGA_BUILD_DIR)
	rm -rf $(INSTALL_HEADERS)
	rm -rf $(LIB_GAPUINO) $(LIB_AI_DECK) $(LIB_GAPOC_A) $(LIB_VEGA)
	rm -rf $(CURDIR)/build $(CURDIR)/install
