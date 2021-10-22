# Retrieve current file directory
OPENMP_DIR ?= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

OPENMP_SRC_DIR= $(OPENMP_DIR)/lib
OPENMP_INC_DIR = $(OPENMP_DIR)/include

OPENMP_SRCS = $(wildcard $(OPENMP_SRC_DIR)/*.c)
OPENMP_INC = $(OPENMP_INC_DIR)
