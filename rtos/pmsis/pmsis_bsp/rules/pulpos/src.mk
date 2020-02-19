ifdef PULPOS_BOARD
BOARD_UPPER = $(shell echo $(PULPOS_BOARD) | tr 'a-z' 'A-Z')
PULP_CFLAGS += -DCONFIG_$(BOARD_UPPER)
endif

ifdef PULPOS_BOARD_VERSION
BOARD_VERSION_UPPER = $(shell echo $(PULPOS_BOARD_VERSION) | tr 'a-z' 'A-Z')
PULP_CFLAGS += -DCONFIG_BOARD_VERSION_$(BOARD_VERSION_UPPER)
endif

ifndef PULPOS_BOARD_PROFILE
PULPOS_BOARD_PROFILE = default
endif

BOARD_PROFILE_UPPER = $(shell echo $(PULPOS_BOARD_PROFILE) | tr 'a-z' 'A-Z')
PULP_CFLAGS += -DCONFIG_PROFILE_$(BOARD_PROFILE_UPPER)




ifeq '$(CONFIG_SPIRAM)' '1'
PULP_SRCS += ram/ram.c ram/alloc_extern.c ram/spiram/spiram.c
CONFIG_BSP = 1
CONFIG_SPIM = 1
endif


ifeq '$(CONFIG_HYPERRAM)' '1'
PULP_SRCS += ram/ram.c ram/alloc_extern.c ram/hyperram/hyperram.c
CONFIG_BSP = 1
CONFIG_HYPER = 1
endif


ifeq '$(CONFIG_BSP)' '1'
PULP_SRCS += bsp/$(PULPOS_BOARD).c
endif
