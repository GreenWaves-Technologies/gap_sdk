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

LIBSFLAGS     = -nostartfiles -nostdlib
LDFLAGS       = -T$(GAP_SDK_HOME)/tools/ld/link.gap8.ld
RISCV_FLAGS   = -march=rv32imcxgap8 -mPE=8 -mFC=1 -D__riscv__
WRAP_FLAGS    = -Wl,--gc-sections
GAP_FLAGS	 += -D__pulp__

# The pre-processor and compiler options.
# Users can override those variables from the command line.
COMMON        = -Os -g -fno-jump-tables -fno-tree-loop-distribute-patterns -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS)

ASMFLAGS      = $(COMMON) -DLANGUAGE_ASSEMBLY -MMD -MP -c

BOOTFLAGS	  = -Os -g -DUSE_AES -fno-jump-tables -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections $(RISCV_FLAGS) $(GAP_FLAGS) -MMD -MP -c

CFLAGS        = $(COMMON) -MMD -MP -c

# Final binary
#------------------------------------------
LIB           = $(TARGET_INSTALL_DIR)/libs/librt.a
HEADER_DIR    = $(TARGET_INSTALL_DIR)/include/pulp-os
BUILD_RT  	  ?= $(BUILD_DIR)/rt

HEADERS       = $(wildcard $(shell find $(RUNTIME_PATH)/include -name "*.h"))

S_OBJECTS     = $(patsubst %.S, $(BUILD_RT)/%.o, $(wildcard $(shell find $(RUNTIME_PATH)/pulp-rt -name "*.S" | sort)))

C_OBJECTS     = $(patsubst %.c, $(BUILD_RT)/%.o, $(wildcard $(shell find $(RUNTIME_PATH)/pulp-rt -name "*.c")))

T_OBJECTS_C   = $(patsubst %.c, $(BUILD_RT)/%.o, $(PULP_APP_FC_SRCS) $(PULP_APP_SRCS))

OBJECTS       = $(S_OBJECTS) $(C_OBJECTS)

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

# Rules for creating rt lib (.d).
#------------------------------------------
$(C_OBJECTS) : $(BUILD_RT)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(S_OBJECTS) : $(BUILD_RT)/%.o : %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASMFLAGS) $< $(INC_PATH) -o $@

$(T_OBJECTS_C) : $(BUILD_RT)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(PULP_CFLAGS) $(TCFLAGS) $< $(INC_PATH) -MD -MF $(basename $@).d -o $@

$(LIB): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) -r $(LIB) $(OBJECTS)

all: install_headers $(OBJECTS) $(LIB)





