TILER_VER=3.0.8
export TILER_LIB=libtile.${TILER_VER}.a
ifdef GAP_SDK_HOME
export TILER_URL=$(GAP_SDK_HOME)/.tiler_url
else
export TILER_URL=.tiler_url
endif

all: Autotiler/LibTile.a

clean:
	rm -rf Autotiler/LibTile*
	rm -f $(TILER_URL)

ifeq (,$(wildcard $(TILER_URL)))
$(TILER_URL): get_tiler.py
	python3 get_tiler.py
endif

Autotiler/LibTile.a: $(TILER_URL)
	mkdir -p Autotiler
	rm -rf Autotiler/LibTile*
	./download_tiler.sh

.PHONY: all clean
