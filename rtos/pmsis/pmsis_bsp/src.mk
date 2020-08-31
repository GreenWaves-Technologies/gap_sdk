BSP_READFS_SRC = fs/read_fs/read_fs.c
BSP_HOSTFS_SRC = fs/host_fs/semihost.c fs/host_fs/host_fs.c
BSP_LFS_SRC = fs/lfs/lfs.c fs/lfs/lfs_util.c fs/lfs/pi_lfs.c
BSP_FS_SRC = fs/fs.c
BSP_FLASH_SRC = flash/flash.c partition/partition.c partition/flash_partition.c \
  crc/md5.c
BSP_HYPERFLASH_SRC = flash/hyperflash/hyperflash.c
BSP_SPIFLASH_SRC = flash/spiflash/spiflash.c
BSP_HYPERRAM_SRC = ram/hyperram/hyperram.c
BSP_SPIRAM_SRC = ram/spiram/spiram.c
BSP_RAM_SRC = ram/ram.c ram/alloc_extern.c
BSP_OTA_SRC = ota/ota.c ota/ota_utility.c ota/updater.c
BSP_BOOTLOADER_SRC = bootloader/bootloader_utility.c
BSP_NINA_SRC = transport/transport.c transport/nina_w10/nina_w10.c
BSP_24XX1025_SRC = eeprom/24XX1025.c
BSP_VIRTUAL_EEPROM_SRC = eeprom/virtual_eeprom.c
BSP_MRAM_SRC = flash/mram/mram-v2.c

COMMON_SRC = \
  $(BSP_FLASH_SRC) \
  $(BSP_FS_SRC) \
  $(BSP_LFS_SRC) \
  $(BSP_READFS_SRC) \
  $(BSP_HOSTFS_SRC) \
  $(BSP_OTA_SRC) \
  $(BSP_BOOTLOADER_SRC)

VEGA_SRC = \
  $(COMMON_SRC) \
  bsp/vega.c \
  camera/camera.c \
  camera/himax/himax.c \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC)

GAP9_SRC = \
  $(COMMON_SRC) \
  eeprom/24XX1025.c \
  $(BSP_VIRTUAL_EEPROM_SRC)\
  bsp/gap9_v2.c \
  camera/camera.c \
  camera/himax/himax.c \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_RAM_SRC)

WOLFE_SRC = \
  $(COMMON_SRC) \
  bsp/wolfe.c \
  camera/camera.c \
  camera/himax/himax.c \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC)

GAPUINO_SRC = \
  $(COMMON_SRC) \
  bsp/gapuino.c \
  camera/camera.c \
  camera/himax/himax.c \
  camera/ov7670/ov7670.c \
  camera/gc0308/gc0308.c \
  camera/ov5640/ov5640.c \
  camera/pixart/pixart.c \
  display/display.c \
  display/ili9341/ili9341.c \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_NINA_SRC) \
  $(BSP_RAM_SRC)

AI_DECK_SRC = \
  $(COMMON_SRC) \
  bsp/ai_deck.c \
  camera/camera.c \
  camera/himax/himax.c \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_NINA_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC)

GAPOC_A_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_a.c \
  camera/camera.c \
  camera/mt9v034/mt9v034.c \
  $(BSP_HYPERFLASH_SRC) \
  transport/transport.c \
  transport/nina_w10/nina_w10.c \
  display/display.c \
  display/ili9341/ili9341.c \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_RAM_SRC) \
  ble/ble.c \
  ble/nina_b112/nina_b112.c \
  ble/nina_b112/nina_b112_old.c

ifeq ($(TARGET_CHIP), GAP8)
GAPOC_B_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_b.c \
  $(BSP_HYPERFLASH_SRC) \
  display/display.c \
  display/ili9341/ili9341.c \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC) \
  ble/ble.c \
  ble/nina_b112/nina_b112.c \
  ble/nina_b112/nina_b112_old.c \
  camera/pixart/pixart.c
else
GAPOC_B_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_b_v2.c \
  camera/camera.c \
  $(BSP_HYPERFLASH_SRC) \
  transport/transport.c \
  display/display.c \
  display/ili9341/ili9341.c \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC) \
  ble/ble.c \
  ble/nina_b112/nina_b112.c \
  ble/nina_b112/nina_b112_old.c \
  camera/thermeye/thermeye.c \
  camera/ov5640/ov5640.c
endif				# TARGET_CHIP
