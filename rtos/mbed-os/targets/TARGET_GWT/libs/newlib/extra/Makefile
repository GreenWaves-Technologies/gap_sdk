LIBS   =
CC     = riscv32-unknown-elf-gcc
AR     = riscv32-unknown-elf-ar
CFLAGS = -O3 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -fdata-sections \
	-ffunction-sections -fno-jump-tables -fno-tree-loop-distribute-patterns -march=rv32imcxgap8 -mPE=8 -mFC=1 \
	-DPREFER_SIZE_OVER_SPEED -D__STD_C  -DDEFINE_MALLOC -DDEFINE_FREE -DINTERNAL_NEWLIB -DHAVE_INITFINI_ARRAY

BUILDDIR = $(shell pwd)/BUILD
LIBC     = libc.a

OBJECTS  = $(patsubst %.c, $(BUILDDIR)/%.o, $(wildcard $(shell find . -name "*.c")))
HEADERS  = $(wildcard $(shell find . -name "*.h"))
INC      = $(shell pwd)/string
INC_PARAMS=$(foreach d, $(INC), -I$d)

all: dir $(OBJECTS) libc_ar libc_ar_move

dir:
	mkdir -p $(BUILDDIR)


libc_ar:
	$(AR) rvs $(BUILDDIR)/$(LIBC) $(OBJECTS)

libc_ar_move:
	$(AR) d ../$(LIBC) lib_a-exit.o
	$(AR) d ../$(LIBC) lib_a-sprintf.o
	$(AR) d ../$(LIBC) lib_a-vsprintf.o
	$(AR) d ../$(LIBC) lib_a-snprintf.o
	$(AR) d ../$(LIBC) lib_a-vsnprintf.o
	$(AR) d ../$(LIBC) lib_a-assert.o
	$(AR) r ../$(LIBC) $(OBJECTS)

$(OBJECTS) : $(BUILDDIR)/%.o : %.c #$(HEADERS)
	mkdir -p $(dir $@)
	$(CC) -MMD -MP  -c $< -o $@ $(CFLAGS) $(INC_PARAMS)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(BUILDDIR)/$(LIBC)
	rm -rf $(BUILDDIR)

