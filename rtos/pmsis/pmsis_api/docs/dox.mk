CHIP ?= gap8

clean:
	rm -rf ./doc
	rm -rf pmsis.html

all:
	cat pmsis.dxy.in | sed s/@CHIP_CONFIG@/$(CHIP)/ > pmsis.dxy
	doxygen pmsis.dxy
	ln -s ${CURDIR}/doc/html/index.html pmsis.html
