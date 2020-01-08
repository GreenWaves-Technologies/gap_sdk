export GAPUINO_SRC
export GAPOC_A_SRC
export GAPOC_B_SRC
export AI_DECK_SRC
export WOLFE_SRC
export VEGA_SRC

all:
ifdef PULPOS
	make -f rules/pulpos.mk build install
endif

clean:
ifdef PULPOS
	make -f rules/pulpos.mk clean
endif

header:
ifdef PULPOS
	make -f rules/pulpos.mk header
endif
ifdef SRC
	mkdir -p $(TARGET_INSTALL_DIR)/src/pmsis_bsp
	cp -r bsp camera display flash ram fs include zephyr $(TARGET_INSTALL_DIR)/src/pmsis_bsp
endif