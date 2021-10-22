include $(OPENMP_DIR)/src.mk

ifeq '$(CONFIG_OPENMP)' '1'
PULP_SRCS += $(OPENMP_SRCS)
PULP_CFLAGS += -I$(OPENMP_INC) -fopenmp
endif
