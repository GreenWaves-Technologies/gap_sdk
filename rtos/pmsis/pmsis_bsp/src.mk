SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

BSP_READFS_SRC = fs/read_fs/read_fs.c
BSP_HOSTFS_SRC = fs/host_fs/host_fs.c
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
BSP_OSPI_FLASH_SRC = flash/spiflash/atxp032.c
BSP_OSPI_RAM_SRC = ram/spiram/aps25xxxn.c
BSP_CAMERA_SRC = camera/camera.c
BSP_HIMAX_SRC = camera/himax/himax.c
BSP_HM0360_SRC = camera/hm0360/hm0360.c
BSP_AK4332_SRC = audio/dac/ak4332.c
BSP_TLV320_SRC = audio/adc/tlv320.c
BSP_FXL6408_SRC = gpio/fxl6408.c
BSP_ADC_ADS1014_SRC = adc/ads1014.c

include $(SELF_DIR)/ble/nina_b112/nina_b112.mk

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
  $(BSP_CAMERA_SRC) \
  $(BSP_HIMAX_SRC) \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC) \
  eeprom/m24c02.c \
  $(BSP_24XX1025_SRC)

BSP_GAP9_EVK_AUDIO_ADDON = \
  bsp/gap9_evk_audio_addon.c

GAP9_SRC = \
  $(COMMON_SRC) \
  eeprom/24XX1025.c \
  $(BSP_VIRTUAL_EEPROM_SRC)\
  bsp/gap9_v2.c \
  $(BSP_CAMERA_SRC) \
  $(BSP_HIMAX_SRC) \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_MRAM_SRC) \
  $(BSP_OSPI_FLASH_SRC)

WOLFE_SRC = \
  $(COMMON_SRC) \
  bsp/wolfe.c \
  $(BSP_CAMERA_SRC) \
  $(BSP_HIMAX_SRC) \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC)

GAPUINO_SRC = \
  $(COMMON_SRC) \
  bsp/gapuino.c \
  $(BSP_CAMERA_SRC) \
  $(BSP_HIMAX_SRC) \
  $(BSP_HM0360_SRC)\
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
  $(BSP_CAMERA_SRC) \
  $(BSP_HIMAX_SRC) \
  $(BSP_HYPERFLASH_SRC) \
  $(BSP_NINA_SRC) \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC)

GAP9_EVK_SRC = \
  $(COMMON_SRC) \
  $(BSP_MRAM_SRC) \
  bsp/gap9_evk.c

GAPOC_A_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_a.c \
  $(BSP_CAMERA_SRC) \
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
  $(BSP_BLE_NINA_B112_SRC)

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
  $(BSP_BLE_NINA_B112_SRC) \
  camera/pixart/pixart.c
else
GAPOC_B_SRC = \
  $(COMMON_SRC) \
  bsp/gapoc_b_v2.c \
  $(BSP_CAMERA_SRC) \
  $(BSP_HYPERFLASH_SRC) \
  transport/transport.c \
  display/display.c \
  display/ili9341/ili9341.c \
  $(BSP_HYPERRAM_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_RAM_SRC) \
  $(BSP_BLE_NINA_B112_SRC) \
  camera/thermeye/thermeye.c \
  camera/ov5640/ov5640.c
endif				# TARGET_CHIP

ifeq '$(BOARD_NAME)' 'gap9_evk'
# Configure the right spi flash
CONFIG_MX25U51245G=1
CONFIG_APS25XXXN=1
CONFIG_IO_UART_ITF=1
CONFIG_IO_UART_BAUDRATE=115200
endif

ifeq '$(BOARD_NAME)' 'gap9_v2'
# Configure the right spi flash
CONFIG_ATXP032=1
CONFIG_HYPERFLASH=1
CONFIG_HYPERRAM=1
CONFIG_APS25XXXN=1
endif

ifneq (,$(findstring $(BOARD_FEATURES),audio_addon))
	PMSIS_BSP_SRC += $(BSP_GAP9_EVK_AUDIO_ADDON)
	PMSIS_BSP_CFLAGS += -DCONFIG_GAP9_EVK_AUDIO_ADDON=1
endif

CONFIG_OCTOSPI = 1
ifeq '$(CONFIG_AK4332)' '1'
PMSIS_BSP_SRC += $(BSP_AK4332_SRC)
CONFIG_FXL6408 = 1
CONFIG_I2C = 1
PMSIS_BSP_CFLAGS += -DCONFIG_AK4332=1
endif

ifeq '$(CONFIG_TLV320)' '1'
PMSIS_BSP_SRC += $(BSP_TLV320_SRC)
CONFIG_FXL6408 = 1
CONFIG_I2C = 1
PMSIS_BSP_CFLAGS += -DCONFIG_TLV320=1
endif

ifeq '$(CONFIG_FXL6408)' '1'
PMSIS_BSP_SRC += $(BSP_FXL6408_SRC)
PMSIS_BSP_CFLAGS += -DCONFIG_FXL6408=1
CONFIG_I2C = 1
endif

ifeq '$(CONFIG_MX25U51245G)' '1'
PMSIS_BSP_SRC += flash/spiflash/mx25u51245g.c
CONFIG_FLASH = 1
CONFIG_OCTOSPI = 1
endif

ifeq '$(CONFIG_APS25XXXN)' '1'
PMSIS_BSP_SRC += ram/spiram/aps25xxxn.c
CONFIG_RAM = 1
CONFIG_OCTOSPI = 1
endif

ifeq '$(CONFIG_RAM)' '1'
PMSIS_BSP_SRC += $(BSP_RAM_SRC)
CONFIG_BSP = 1
endif

ifeq '$(CONFIG_ADS1014)' '1'
PMSIS_BSP_SRC += $(BSP_ADC_ADS1014_SRC)
PMSIS_BSP_CFLAGS += -DCONFIG_ADS1014=1
endif
