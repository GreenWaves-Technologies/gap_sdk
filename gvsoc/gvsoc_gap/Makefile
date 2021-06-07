export VP_ROOT_DIR = $(CURDIR)

clean:
	$(MAKE) -C models clean

build: $(TARGETS)
	$(MAKE) -C models props ARCHI_DIR=$(ARCHI_DIR)
	$(MAKE) -C models build ARCHI_DIR=$(ARCHI_DIR) $(GV_MAKEFLAGS)

checkout:
	git submodule update --init
