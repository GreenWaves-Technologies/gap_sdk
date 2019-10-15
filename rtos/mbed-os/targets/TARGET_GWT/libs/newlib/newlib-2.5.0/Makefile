newlib_version :=2.5.0
srcdir := $(shell pwd)
builddir := $(shell pwd)

INSTALL_DIR := $(srcdir)/riscv-newlib-lib

newlib_url := ftp://sourceware.org/pub/newlib/newlib-$(newlib_version).tar.gz

XLEN := 32

make_tuple = riscv$(1)-unknown-$(2)
NEWLIB_TUPLE ?= $(call make_tuple,$(XLEN),elf)

CFLAGS_FOR_TARGET := $(CFLAGS_FOR_TARGET_EXTRA) -mcmodel=medlow -D__riscv_xlen=32  -D__riscv_float_abi_soft \
		-mPE=8 -mFC=1

CONFIGURE_HOST   =

all: get-libc build-libc create-libc.a

.PHONY: build-libc

get-libc:
	if [ -d ./riscv-newlib ]; then \
		echo "Already have newlib source !";\
	else	                                   \
		git clone git@github.com:owenchj/riscv-newlib.git; \
	fi

build-libc: stamps/build-newlib stamps/build-newlib-nano \
	stamps/merge-newlib-nano


stamps/build-newlib: $(srcdir)/riscv-newlib
	rm -rf $@ $(notdir $@)
	mkdir $(notdir $@)
	cd $(notdir $@) && $</configure \
		--target=$(NEWLIB_TUPLE) \
		$(CONFIGURE_HOST) \
		--prefix=$(INSTALL_DIR) \
		--enable-newlib-io-long-double \
		--enable-newlib-io-long-long \
		--enable-newlib-io-c99-formats \
		CFLAGS_FOR_TARGET="-Os $(CFLAGS_FOR_TARGET)"
	$(MAKE) -C $(notdir $@)
	$(MAKE) -C $(notdir $@) install
	mkdir -p $(dir $@) && touch $@

stamps/build-newlib-nano: $(srcdir)/riscv-newlib
	rm -rf $@ $(notdir $@)
	mkdir $(notdir $@)
	cd $(notdir $@) && $</configure \
		--target=$(NEWLIB_TUPLE) \
		$(CONFIGURE_HOST) \
		--prefix=$(builddir)/install-newlib-nano \
		--enable-newlib-reent-small \
		--disable-newlib-fvwrite-in-streamio \
		--disable-newlib-fseek-optimization \
		--disable-newlib-wide-orient \
		--enable-newlib-nano-malloc \
		--disable-newlib-unbuf-stream-opt \
		--enable-lite-exit \
		--enable-newlib-global-atexit \
		--enable-newlib-nano-formatted-io \
		--disable-newlib-supplied-syscalls \
		--disable-nls \
		--enable-newlib-register-fini \
		CFLAGS_FOR_TARGET="-Os -ffunction-sections -fdata-sections $(CFLAGS_FOR_TARGET)"
	$(MAKE) -C $(notdir $@)
	$(MAKE) -C $(notdir $@) install
	mkdir -p $(dir $@) && touch $@

stamps/merge-newlib-nano: stamps/build-newlib-nano stamps/build-newlib
# Copy nano library files into newlib install dir.
	cp $(builddir)/install-newlib-nano/$(NEWLIB_TUPLE)/lib/libc.a \
		$(INSTALL_DIR)/$(NEWLIB_TUPLE)/lib/libc_nano.a; \
	cp $(builddir)/install-newlib-nano/$(NEWLIB_TUPLE)/lib/libg.a \
		$(INSTALL_DIR)/$(NEWLIB_TUPLE)/lib/libg_nano.a; \
	cp $(builddir)/install-newlib-nano/$(NEWLIB_TUPLE)/lib/libgloss.a\
		$(INSTALL_DIR)/$(NEWLIB_TUPLE)/lib/libgloss_nano.a; \

# Copy nano header files into newlib install dir.
	mkdir -p $(INSTALL_DIR)/$(NEWLIB_TUPLE)/include/newlib-nano; \
	cp $(builddir)/install-newlib-nano/$(NEWLIB_TUPLE)/include/newlib.h \
		$(INSTALL_DIR)/$(NEWLIB_TUPLE)/include/newlib-nano/newlib.h; \
	mkdir -p $(dir $@) && touch $@

create-libc.a:
	cp $(INSTALL_DIR)/$(NEWLIB_TUPLE)/lib/libc.a ../libc.a

clear/build:
	rm -rf stamps
	rm -rf build-newlib
	rm -rf build-newlib-nano
	rm -rf install-newlib-nano
