#
# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.
#

CC = gcc
#src = $(wildcard *.c)
src = main.c layers.c
obj = $(src:.c=.o)

CFLAGS = -g3 -O0 --std=c99
LDFLAGS = -lm

EXEC=genSSDparams

all: $(EXEC)

$(EXEC): $(obj)
	$(CC) -g3 -o $@ $^ $(LDFLAGS)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(obj) $(EXEC)

#.PHONY: cnn_test clean run
