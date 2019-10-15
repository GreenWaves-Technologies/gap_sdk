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

FLASHER_INC = $(CURDIR)/include
INSTALL_DIR ?= $(CURDIR)/install
PULP_INC_DIR = $(CURDIR)/include
PULP_LIB_DIR		= $(CURDIR)/lib
BUILD_DIR   = $(CURDIR)/build
RELEASE_TYPE ?= Debug
# propagate verbose for debugging
VERBOSE ?= 0
BUILDDIR      = $(BUILD_DIR)/flasher_build

$(info #### Building in $(BUILD_DIR))
$(info #### Release type is $(RELEASE_TYPE))
$(info #### Installing to $(INSTALL_DIR))
$(info #### Installing target files to $(PULP_INC_DIR))

BIN_DIR 	  = $(INSTALL_DIR)/bin
BIN           = $(BIN_DIR)/flasher
BIN_FUSER	  = $(BIN_DIR)/fuser

SRCS 		  = flasher.c hyper_flash.c hyper_flash_commands.c
SRC_DIR 	  = $(CURDIR)/src
OBJECTS   	  = $(patsubst %.c, $(BUILDDIR)/%.o, $(foreach f, $(SRCS), $(SRC_DIR)/$(f)))

INC           = $(FLASHER_INC)

INC_DEFINE    = -include $(PULP_INC_DIR)/gap_config.h

INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE)

# The linker options.
LIBS          += -L$(PULP_LIB_DIR)/gap -lrt -lrtio -lrt -lgcc
INSTALL_LDDIR = $(INSTALL_DIR)/ld
LDFLAGS       += -T$(INSTALL_LDDIR)/link.gap8.ld -T$(INSTALL_LDDIR)/gapuino.conf.ld

LIBSFLAGS     += -nostartfiles -nostdlib
WRAP_FLAGS    = -Wl,--gc-sections

PULP_CFLAGS += -O3 #-DDEBUG -DfileIO

RISCV_FLAGS   = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__riscv__
GAP_FLAGS	 += -D__pulp__ -DCONFIG_GAP

CFLAGS        = -Os -g -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

TCFLAGS       = -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

BIN_FILES = $(shell find bin -type f)
LD_FILES = $(shell find ld -type f)

define declareInstallFile
$(INSTALL_DIR)/$(1): $(1)
	install -D $(1) $$@
INSTALL_FILES += $(INSTALL_DIR)/$(1)
endef

$(foreach file, $(BIN_FILES) $(LD_FILES), $(eval $(call declareInstallFile,$(file))))

all: dir $(INSTALL_FILES) $(OBJECTS) $(BIN)

dir:
	mkdir -p $(BUILDDIR)
	mkdir -p $(INSTALL_DIR)
	mkdir -p $(BIN_DIR)

$(OBJECTS) : $(BUILDDIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $(SRC_DIR)/fuser.c $(INC_PATH) -MD -MF $(BUILD_DIR)/fuser.d -o $(BUILD_DIR)/fuser.o

$(BIN): $(OBJECTS)
	$(CC) -MMD -MP $(WRAP_FLAGS) $(PULP_CFLAGS) -o $(BIN) $(OBJECTS) $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)


$(BIN_FUSER): $(BUILD_DIR)
	$(CC) -MMD -MP $(WRAP_FLAGS) $(PULP_CFLAGS) -o $(BIN_FUSER) $(BUILD_DIR)/fuser.o $(LIBS) $(LDFLAGS) $(LIBSFLAGS) $(INC_DEFINE)

fuser: $(BIN_FUSER)

install: all fuser

clean:
	rm -rf $(BIN) $(BUILDDIR)
	rm -rf $(CURDIR)/build $(CURDIR)/install
