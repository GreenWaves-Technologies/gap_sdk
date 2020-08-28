#$(info #### PMSIS IMPLEM PATH : $(PMSIS_IMPLEM_DIR))

# PMSIS implem sources.
ifeq ($(TARGET_CHIP_FAMILY), GAP8)
PMSIS_IMPLEM_GAP8_DIR    = $(PMSIS_IMPLEM_DIR)/pmsis_implem_gap8
-include $(PMSIS_IMPLEM_GAP8_DIR)/pmsis_implem_gap8.mk
else
PMSIS_IMPLEM_GAP9_DIR    = $(PMSIS_IMPLEM_DIR)/pmsis_implem_gap9
-include $(PMSIS_IMPLEM_GAP9_DIR)/pmsis_implem_gap9.mk
endif                           # TARGET_CHIP_FAMILY

PMSIS_RTOS_DIR           ?= $(PMSIS_IMPLEM_DIR)/pmsis_rtos
-include $(PMSIS_RTOS_DIR)/pmsis_rtos.mk

PMSIS_IMPLEM_SRCS        = $(PMSIS_RTOS_SRCS) \
                           $(PMSIS_IMPLEM_CHIP_SRCS)

PMSIS_IMPLEM_ASM_SRCS    = $(PMSIS_IMPLEM_CHIP_ASM_SRCS)

PMSIS_IMPLEM_INC         = $(PMSIS_IMPLEM_DIR) \
                           $(PMSIS_RTOS_INC) \
                           $(PMSIS_IMPLEM_CHIP_INC)


#$(info #### Include : $(PMSIS_IMPLEM_SRCS))
#$(info #### Include : $(PMSIS_IMPLEM_INC))
