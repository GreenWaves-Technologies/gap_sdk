INSTALL_FILES += $(shell find include -name *.h)

PULP_LIBS += gaplib

PULP_LIB_FC_SRCS_gaplib = $(SRC)

PULP_CFLAGS += -O2 -g -I$(CURDIR)/include
PULP_CFLAGS += -Wextra -Wall -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef

include $(PULP_SDK_HOME)/install/rules/pulp.mk
