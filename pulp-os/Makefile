ifndef GAP_SDK_HOME
    $(error Please run 'source sourceme.sh' in gap_sdk first)
endif

CD = cd
CP = cp -rf
RM = rm -rf
MKDIR = mkdir

# The C program compiler.
CXX           = riscv32-unknown-elf-g++
CC            = riscv32-unknown-elf-gcc
AR            = riscv32-unknown-elf-ar
OBJDUMP       = riscv32-unknown-elf-objdump

LDFLAGS       = -T$(GAP_SDK_HOME)/tools/ld/link.gap8.ld

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__riscv__ -Os -g -fno-jump-tables -Werror -fno-tree-loop-distribute-patterns  -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections

ASMFLAGS      = $(COMMON) -DLANGUAGE_ASSEMBLY -MMD -MP -c

CFLAGS        = $(COMMON) -MMD -MP -c

# Final binary
#------------------------------------------
RT_LIB        = $(TARGET_INSTALL_DIR)/libs/librt.a
IO_LIB		  = $(TARGET_INSTALL_DIR)/libs/libio.a

HEADER_DIR    = $(TARGET_INSTALL_DIR)/include/pulp-os
BUILD_RT  	  ?= $(BUILD_DIR)/rt
COMMON_SRC	  += $(RUNTIME_PATH)/pulp-rt/rt_conf.c $(RUNTIME_PATH)/pulp-rt/rt_pad_conf.c

HEADERS       = $(wildcard $(shell find $(RUNTIME_PATH)/include -name "*.h"))

RT_S_OBJECTS     = $(patsubst %.S, $(BUILD_RT)/%.o, $(wildcard $(shell find $(RUNTIME_PATH)/pulp-rt -name "*.S" | sort)))

RT_C_OBJECTS     = $(patsubst %.c, $(BUILD_RT)/%.o, $(wildcard $(shell find $(RUNTIME_PATH)/pulp-rt/kernel -name "*.c") $(shell find $(RUNTIME_PATH)/pulp-rt/drivers -name "*.c") $(COMMON_SRC)))

RT_OBJECTS       = $(RT_S_OBJECTS) $(RT_C_OBJECTS)

IO_C_OBJECTS     = $(patsubst %.c, $(BUILD_RT)/%.o, $(wildcard $(shell find $(RUNTIME_PATH)/pulp-rt/libs/io -name "*.c")))

INC_DEFINE    = -include $(TARGET_INSTALL_DIR)/include/pulp-os/gap_config.h

INC           = $(TARGET_INSTALL_DIR)/include/pulp-os \
				$(TARGET_INSTALL_DIR)/include/pulp-os/io \
				$(GAP_SDK_HOME)/tools/pulp-debug-bridge/include
INC_PATH      = $(foreach d, $(INC), -I$d)  $(INC_DEFINE)

$(HEADER_DIR):
	mkdir -p $@

install_headers: $(HEADER_DIR)
	$(CP) $(GAP_SDK_HOME)/pulp-os/include/* $(HEADER_DIR)
	install -D $(GAP_SDK_HOME)/pulp-os/include/Gap8.h $(TARGET_INSTALL_DIR)/include

# Rules for creating the libs.
#------------------------------------------
$(RT_C_OBJECTS) : $(BUILD_RT)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(INC_PATH) -o $@

$(RT_S_OBJECTS) : $(BUILD_RT)/%.o : %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASMFLAGS) $< $(INC_PATH) -o $@

$(RT_LIB): $(RT_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) -r $(RT_LIB) $(RT_OBJECTS)

$(IO_C_OBJECTS) : $(BUILD_RT)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(INC_PATH) -o $@

$(IO_LIB): $(IO_C_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) -r $(IO_LIB) $(IO_C_OBJECTS)

all: install_headers $(OBJECTS) $(RT_LIB) $(IO_LIB)





