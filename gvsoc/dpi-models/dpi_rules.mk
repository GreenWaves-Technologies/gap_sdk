DPI_BUILD_DIR = $(ROOT_DPI_BUILD_DIR)/models

DPI_INSTALL_PATH ?= $(INSTALL_DIR)/lib

DPI_MAKEFILE_LIST = $(addsuffix /Makefile,$(DPI_DIRS))

CPP=g++

DPI_MODEL_CFLAGS=-MMD -MP -O2 -g -fpic -std=c++11 -I$(INSTALL_DIR)/include
DPI_MODEL_LDFLAGS=-O2 -g -shared -L$(INSTALL_DIR)/lib

DPI_MODEL_CFLAGS += -Werror -Wfatal-errors
DPI_MODEL_LDFLAGS += -Werror -Wfatal-errors


DPI_MODEL_EXT=.so



include $(DPI_MAKEFILE_LIST)


define declare_model

-include $(DPI_BUILD_DIR)/$(1).d

$(DPI_BUILD_DIR)/$(1)$(DPI_MODEL_EXT): $($(1)_SRCS) $($(1)_DEPS)
	@mkdir -p `dirname $$@`
	$(CPP) $($(1)_SRCS) -o $$@ $($(1)_CFLAGS) $(DPI_MODEL_CFLAGS) $($(1)_LDFLAGS) $(DPI_MODEL_LDFLAGS)

$(DPI_INSTALL_PATH)/$(1)$(DPI_MODEL_EXT): $(DPI_BUILD_DIR)/$(1)$(DPI_MODEL_EXT)
	install -D $$^ $$@

DPI_INSTALL_TARGETS += $(DPI_INSTALL_PATH)/$(1)$(DPI_MODEL_EXT)

endef


$(foreach model, $(DPI_MODELS), $(eval $(call declare_model,$(model))))


dpi_build: $(DPI_INSTALL_TARGETS)

dpi_clean:
	rm -rf $(DPI_BUILD_DIR)