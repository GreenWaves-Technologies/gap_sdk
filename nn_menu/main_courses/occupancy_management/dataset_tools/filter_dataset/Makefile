### Copyright (C) 2020 GreenWaves Technologies
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

CC = gcc
src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS  = -O2 --std=c99 -Ishutterless_lib/
LDFLAGS = -L. -lshutterless -lm

EXEC=ImgsFilter


$(EXEC): $(obj)
	$(CC) $(CFLAGS) -g3 -o $@ $^ $(LDFLAGS)

all: $(EXEC)

run:
	mkdir -p filtered_raw
	mkdir -p filtered_pgm
	rm -rf filtered_raw/*
	rm -rf filtered_pgm/*
	./$(EXEC) ../gap8_capture_raw_images/offset_img/Calibration.bin ../gap8_capture_raw_images/dump_out_imgs/

clean:
	rm -f $(obj) $(EXEC) 

.DEFAULT_GOAL := all
.PHONY: all run clean
