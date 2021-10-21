PULP_PROPERTIES += udma/cpi/version pulp_chip_family

include $(TARGET_INSTALL_DIR)/rules/pulp_properties.mk

INSTALL_FILES += $(shell find include -name *.h)

ifeq '$(pulp_chip_family)' 'gap'
PULP_LIBS += pitools_gapoc_a pitools_gapuino pitools_ai_deck
endif

ifeq '$(pulp_chip_family)' 'vega'
PULP_LIBS += pitools_vega
endif

PULP_LIB_FC_SRCS_pitools_vega = $(SRC)
PULP_LIB_TARGET_NAME_pitools_vega = vega/libpitools.a
PULP_LIB_CFLAGS_pitools_vega = -DCONFIG_VEGA

PULP_LIB_FC_SRCS_pitools_gapoc_a = $(SRC)
PULP_LIB_TARGET_NAME_pitools_gapoc_a = gapoc_a/libpitools.a
PULP_LIB_CFLAGS_pitools_gapoc_a = -DCONFIG_GAPOC_A

PULP_LIB_FC_SRCS_pitools_gapuino = $(SRC)
PULP_LIB_TARGET_NAME_pitools_gapuino = gapuino/libpitools.a
PULP_LIB_CFLAGS_pitools_gapuino = -DCONFIG_GAPUINO

PULP_LIB_FC_SRCS_pitools_ai_deck = $(SRC)
PULP_LIB_TARGET_NAME_pitools_ai_deck = ai_deck/libpitools.a
PULP_LIB_CFLAGS_pitools_ai_deck = -DCONFIG_AI_DECK

PULP_CFLAGS += -Os -g -I$(CURDIR)/include
PULP_CFLAGS += -Wextra -Wall -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef

include $(PULP_SDK_HOME)/install/rules/pulp.mk
