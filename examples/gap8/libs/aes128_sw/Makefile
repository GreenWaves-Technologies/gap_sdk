APP              = aes
APP_SRCS = AesLib.c  main.c
APP_CFLAGS = -O3

output ?= emul_build
emul:
	mkdir -p $(outpath)
	gcc $(CFLAGS) -o $(outpath)/$(output) $(SRC)

include $(RULES_DIR)/pmsis_rules.mk
