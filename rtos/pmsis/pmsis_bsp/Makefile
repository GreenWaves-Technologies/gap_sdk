COMMON_SRC = \
  flash/flash.c \
  fs/fs.c \
  fs/read_fs/read_fs.c \
  fs/host_fs/semihost.c \
  fs/host_fs/host_fs.c \
  partition/partition.c \

VEGA_SRC = \
  $(COMMON_SRC) \
  bsp/vega.c \
  camera/camera.c \
  camera/himax/himax.c \
  flash/hyperflash/hyperflash.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c

GAP9_SRC = \
  $(COMMON_SRC) \
  bsp/gap9.c \
  camera/camera.c \
  camera/himax/himax.c \
  flash/hyperflash/hyperflash.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c

WOLFE_SRC = \
  $(COMMON_SRC) \
  bsp/wolfe.c \
  camera/camera.c \
  camera/himax/himax.c \
  flash/hyperflash/hyperflash.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c

GAPUINO_SRC = \
  $(COMMON_SRC) \
  bsp/gapuino.c \
  camera/camera.c \
  camera/himax/himax.c \
  display/display.c \
  display/ili9341/ili9341.c \
  flash/hyperflash/hyperflash.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c

AI_DECK_SRC = \
  $(COMMON_SRC) \
  bsp/ai_deck.c \
  camera/camera.c \
  camera/himax/himax.c \
  flash/hyperflash/hyperflash.c \
  transport/transport.c \
  transport/nina_w10/nina_w10.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c

GAPOC_A_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_a.c \
  camera/camera.c \
  camera/mt9v034/mt9v034.c \
  flash/hyperflash/hyperflash.c \
  transport/transport.c \
  display/display.c \
  display/ili9341/ili9341.c \
  transport/nina_w10/nina_w10.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c \
  ble/ble.c \
  ble/nina_b112/nina_b112.c \
  ble/nina_b112/nina_b112_old.c

GAPOC_B_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_b.c \
  camera/camera.c \
  flash/hyperflash/hyperflash.c \
  transport/transport.c \
  display/display.c \
  display/ili9341/ili9341.c \
  ram/hyperram/hyperram.c \
  ram/ram.c \
  ram/alloc_extern.c \
  ble/ble.c \
  ble/nina_b112/nina_b112.c \
  ble/nina_b112/nina_b112_old.c \
  camera/thermeye/thermeye.c

ifdef GAP_SDK_HOME
include $(CURDIR)/rules/gap_sdk.mk
else
include $(CURDIR)/rules/pulp_sdk.mk
endif
