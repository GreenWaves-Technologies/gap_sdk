export SRC

all_target:
ifdef PULPOS
	make -f rules/pulpos.mk build install
endif

clean_target:
ifdef PULPOS
	make -f rules/pulpos.mk clean
endif

header_target:
ifdef PULPOS
	make -f rules/pulpos.mk header
endif
ifdef SRC
	mkdir -p $(TARGET_INSTALL_DIR)/src/pmsis_tools
	cp -r frame_streamer $(TARGET_INSTALL_DIR)/src/pmsis_tools
endif