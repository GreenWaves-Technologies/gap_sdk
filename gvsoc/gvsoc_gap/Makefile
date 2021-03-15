export VP_ROOT_DIR = $(CURDIR)

GV_MAKEFLAGS ?= -j 2

clean:
	make -C models clean

build: $(TARGETS)
	make -C models props ARCHI_DIR=$(ARCHI_DIR)
	make -C models build ARCHI_DIR=$(ARCHI_DIR) $(GV_MAKEFLAGS)

checkout:
	git submodule update --init
