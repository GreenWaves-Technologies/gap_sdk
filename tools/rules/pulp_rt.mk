APP = $(PULP_APP)

$(info $(PULP_APP_SRCS))
PMSIS_OS = pulpos

ifeq '$(PMSIS_OS)' 'pulpos'
ifeq '$(TARGET_CHIP)' 'GAP9_V2'
export USE_PULPOS=1
endif
endif

ifeq '$(USE_PULPOS)' '1'
include $(RULES_DIR)/pmsis_rules.mk
else
include $(RULES_DIR)/pulp_rules.mk
endif