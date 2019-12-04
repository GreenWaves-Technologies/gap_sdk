TILER_VER=2.0.1
export TILER_LIB=libtile.${TILER_VER}.a
ifdef GAP_SDK_HOME
export TILER_URL=$(GAP_SDK_HOME)/.tiler_url
else
export TILER_URL=.tiler_url
endif

all: lib/libtile.a

clean:
	rm -rf lib/libtile*
	rm -f $(TILER_URL)

ifeq (,$(wildcard $(TILER_URL)))
$(TILER_URL): get_tiler.py
	python3 get_tiler.py
endif

lib/libtile.a: $(TILER_URL)
	mkdir -p lib
	rm -rf lib/libtile*
	./download_tiler.sh

.PHONY: all clean
