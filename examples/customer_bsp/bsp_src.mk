GAPOC_C_BSP = $(GAPOC_C_PROJECT_PATH)
PMSIS_BSP_PATH = $(GAP_SDK_HOME)/rtos/pmsis/pmsis_bsp

BSP_READFS_SRC = $(PMSIS_BSP_PATH)/fs/read_fs/read_fs.c
BSP_HOSTFS_SRC = $(PMSIS_BSP_PATH)/fs/host_fs/semihost.c $(PMSIS_BSP_PATH)/fs/host_fs/host_fs.c
BSP_LFS_SRC = $(PMSIS_BSP_PATH)/fs/lfs/lfs.c $(PMSIS_BSP_PATH)/fs/lfs/lfs_util.c $(PMSIS_BSP_PATH)/fs/lfs/pi_lfs.c
BSP_FS_SRC = $(PMSIS_BSP_PATH)/fs/fs.c
BSP_FLASH_SRC = $(PMSIS_BSP_PATH)/flash/flash.c $(PMSIS_BSP_PATH)/partition/partition.c $(PMSIS_BSP_PATH)/partition/flash_partition.c \
  $(PMSIS_BSP_PATH)/crc/md5.c

BSP_HYPERFLASH_SRC = $(PMSIS_BSP_PATH)/flash/hyperflash/hyperflash.c
BSP_HYPERRAM_SRC = $(PMSIS_BSP_PATH)/ram/hyperram/hyperram.c
BSP_SPIFLASH_SRC = $(PMSIS_BSP_PATH)/flash/spiflash/spiflash.c
BSP_SPIRAM_SRC = $(PMSIS_BSP_PATH)/ram/spiram/spiram.c

BSP_RAM_SRC = $(PMSIS_BSP_PATH)/ram/ram.c $(PMSIS_BSP_PATH)/ram/alloc_extern.c
BSP_OTA_SRC = $(PMSIS_BSP_PATH)/ota/ota.c $(PMSIS_BSP_PATH)/ota/ota_utility.c $(PMSIS_BSP_PATH)/ota/updater.c
BSP_BOOTLOADER_SRC = $(PMSIS_BSP_PATH)/bootloader/bootloader_utility.c

BSP_CAM_SRC = $(PMSIS_BSP_PATH)/camera/camera.c $(PMSIS_BSP_PATH)/camera/hm0360/hm0360.c

COMMON_SRC = \
  $(BSP_FLASH_SRC) \
  $(BSP_FS_SRC) \
  $(BSP_LFS_SRC) \
  $(BSP_READFS_SRC) \
  $(BSP_HOSTFS_SRC) \
  $(BSP_OTA_SRC) \
  $(BSP_BOOTLOADER_SRC)

GAPOC_C_SRC = \
  $(COMMON_SRC) \
  $(GAPOC_C_BSP)/bsp/gapoc_c.c \
  $(BSP_SPIFLASH_SRC) \
  $(BSP_SPIRAM_SRC) \
  $(BSP_RAM_SRC) \
  $(BSP_CAM_SRC)

#  $(BSP_HYPERFLASH_SRC) \
#  $(BSP_HYPERRAM_SRC) \


GAPOC_C_BSP_FLAGS += -DCONFIG_SPIFLASH \
					  -DCONFIG_SPIRAM	\
					  -DCONFIG_HM0360 
