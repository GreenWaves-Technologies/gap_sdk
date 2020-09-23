# Hello World

## Requirements

+ Chip: GAP9.
+ OS: Any.
+ Board: Any.
+ Command line: add CONFIG_XIP=1 if using jtag/rtl boot. Can also add it in Makefile.
If boot from flash, configure the right fuses.

## Description

This example is a classic Hello World, but it also test launching some "hello" from various Area
of memories, challenging the icache, and reports errors if needed.
