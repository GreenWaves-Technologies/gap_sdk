# User Test
#------------------------------------

APP              = test
APP_SRCS        += test_fs_to_l3_copy.c
APP_INC         +=
APP_CFLAGS      +=

FILE_BIAS0   = coef/Cifar10_Bias0.dat
FILE_BIAS1   = coef/Cifar10_Bias1.dat
FILE_BIAS2   = coef/Cifar10_Bias2.dat
FILE_FILTER0 = coef/Cifar10_Filter0.dat
FILE_FILTER1 = coef/Cifar10_Filter1.dat
FILE_FILTER2 = coef/Cifar10_Filter2.dat

FLASH_FILES = $(FILE_BIAS0) $(FILE_BIAS1) $(FILE_BIAS2) $(FILE_FILTER0) $(FILE_FILTER1) $(FILE_FILTER2)

PLPBRIDGE_FLAGS += -f $(FLASH_FILES)

# For GVSOC
override runner_args += --config-opt=flash/fs/files=$(CURDIR)/$(FILE_BIAS0) \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FILE_BIAS1) \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FILE_BIAS2) \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FILE_FILTER0) \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FILE_FILTER1) \
                        --config-opt=flash/fs/files=$(CURDIR)/$(FILE_FILTER2)

include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk
