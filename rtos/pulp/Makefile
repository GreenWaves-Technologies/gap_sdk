DEFAULT_EXCLUDES=--exclude=".git*" --exclude="BUILD*" --exclude="build*" --exclude="_build*" --exclude="__pycache__/" --exclude=junit-reports

pulp_configs:
	make -C $(GAP_SDK_HOME)/tools/pulp_tools/pulp-configs all_scons INSTALL_DIR=$(INSTALL_DIR) TARGET_INSTALL_DIR=$(TARGET_INSTALL_DIR)

do.archi: pulp_configs
	if [ -e archi_pulp ]; then \
		cd archi_pulp && \
			export INSTALL_DIR=$(GAP_SDK_HOME)/rtos/pulp/archi/workstation && \
			export TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/rtos/pulp/archi/target && \
			make build; \
	fi

build.archi: do.archi
	cd $(GAP_SDK_HOME)/rtos/pulp/archi/target && rsync -avR * $(TARGET_INSTALL_DIR)
	cd $(GAP_SDK_HOME)/rtos/pulp/archi/workstation && rsync -avR * $(INSTALL_DIR)

artifact.archi: do.archi
	mkdir -p $(ARTIFACT_PATH)/rtos/pulp/archi
	rsync -avR $(DEFAULT_EXCLUDES) archi $(ARTIFACT_PATH)/rtos/pulp

do.hal: pulp_configs
	if [ -e hal_pulp ]; then \
		cd hal_pulp && \
			export TARGET_INSTALL_DIR=$(GAP_SDK_HOME)/rtos/pulp/hal/target && \
			make build; \
	fi

build.hal: do.hal
	cd $(GAP_SDK_HOME)/rtos/pulp/hal/target && rsync -avR * $(TARGET_INSTALL_DIR)

build.gap_archi:
ifeq '$(TARGET_CHIP)' 'GAP9_V2'
	cd $(GAP_SDK_HOME)/rtos/pulp/gap_archi && make build
endif
	
artifact.hal: do.hal
	mkdir -p $(ARTIFACT_PATH)/rtos/pulp/hal
	rsync -avR $(DEFAULT_EXCLUDES) hal $(ARTIFACT_PATH)/rtos/pulp

artifact.pulpos:
	mkdir -p $(ARTIFACT_PATH)/rtos/pulp/pulp-os
	rsync -avR $(DEFAULT_EXCLUDES) pulp-os $(ARTIFACT_PATH)/rtos/pulp

artifact: artifact.archi artifact.hal artifact.pulpos

build.pulprt:
ifneq '$(TARGET_CHIP)' 'GAP9_V2'
	$(MAKE) -C pulp-os -f gap_sdk.mk $(TARGET_NAME)
endif

build: build.archi build.hal build.gap_archi build.pulprt