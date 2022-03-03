set(BSP_READFS_SRC fs/read_fs/read_fs.c)
set(BSP_HOSTFS_SRC fs/host_fs/host_fs.c)
set(BSP_LFS_SRC fs/lfs/lfs.c fs/lfs/lfs_util.c fs/lfs/pi_lfs.c)
set(BSP_FS_SRC fs/fs.c)
set(BSP_FLASH_SRC
    flash/flash.c partition/partition.c partition/flash_partition.c crc/md5.c
    )
set(BSP_HYPERFLASH_SRC flash/hyperflash/hyperflash.c)
set(BSP_SPIFLASH_SRC flash/spiflash/spiflash.c)
set(BSP_HYPERRAM_SRC ram/hyperram/hyperram.c)
set(BSP_SPIRAM_SRC ram/spiram/spiram.c)
set(BSP_RAM_SRC ram/ram.c ram/alloc_extern.c)
set(BSP_OTA_SRC ota/ota.c ota/ota_utility.c ota/updater.c)
set(BSP_BOOTLOADER_SRC bootloader/bootloader_utility.c)
set(BSP_NINA_SRC transport/transport.c transport/nina_w10/nina_w10.c)
set(BSP_24XX1025_SRC eeprom/24XX1025.c)
set(BSP_VIRTUAL_EEPROM_SRC eeprom/virtual_eeprom.c)
set(BSP_MRAM_SRC flash/mram/mram-v2.c)
set(BSP_OSPI_FLASH_SRC flash/spiflash/atxp032.c)
set(BSP_OSPI_RAM_SRC ram/spiram/aps25xxxn.c)
set(BSP_CAMERA_SRC camera/camera.c)
set(BSP_HIMAX_SRC camera/himax/himax.c)
set(BSP_HM0360_SRC camera/hm0360/hm0360.c)
set(BSP_BLE_NINA_B112_SRC ble/ble.c ble/nina_b112/nina_b112.c ble/nina_b112/nina_b112_old.c)

set(COMMON_SRC
  ${BSP_FLASH_SRC}  
  ${BSP_FS_SRC}  
  ${BSP_LFS_SRC}  
  ${BSP_READFS_SRC}  
  ${BSP_HOSTFS_SRC}  
  ${BSP_OTA_SRC}  
  ${BSP_BOOTLOADER_SRC}
  )

set(VEGA_SRC 
  ${COMMON_SRC}  
  bsp/vega.c  
  ${BSP_CAMERA_SRC}  
  ${BSP_HIMAX_SRC}  
  ${BSP_HYPERFLASH_SRC}  
  ${BSP_HYPERRAM_SRC}  
  ${BSP_SPIRAM_SRC}  
  ${BSP_SPIFLASH_SRC}  
  ${BSP_RAM_SRC}  
  eeprom/m24c02.c  
  ${BSP_24XX1025_SRC}
)

set(GAP9_SRC
  ${COMMON_SRC}  
  eeprom/24XX1025.c  
  ${BSP_VIRTUAL_EEPROM_SRC} 
  bsp/gap9_v2.c  
  ${BSP_CAMERA_SRC}  
  ${BSP_HIMAX_SRC}  
  ${BSP_HYPERFLASH_SRC}  
  ${BSP_HYPERRAM_SRC}  
  ${BSP_RAM_SRC}  
  ${BSP_MRAM_SRC}  
  ${BSP_OSPI_FLASH_SRC}  
  ${BSP_OSPI_RAM_SRC}  
  ${BSP_BLE_NINA_B112_SRC}
)

set(GAPUINO_SRC
  ${COMMON_SRC}  
  bsp/gapuino.c  
  ${BSP_CAMERA_SRC}  
  ${BSP_HIMAX_SRC}  
  ${BSP_HM0360_SRC} 
  camera/ov7670/ov7670.c  
  camera/gc0308/gc0308.c  
  camera/ov5640/ov5640.c  
  camera/pixart/pixart.c  
  display/display.c  
  display/ili9341/ili9341.c  
  ${BSP_HYPERFLASH_SRC}  
  ${BSP_HYPERRAM_SRC}  
  ${BSP_SPIRAM_SRC}  
  ${BSP_SPIFLASH_SRC}  
  ${BSP_NINA_SRC}  
  ${BSP_RAM_SRC}
)

set(AI_DECK_SRC
  ${COMMON_SRC}  
  bsp/ai_deck.c  
  ${BSP_CAMERA_SRC}  
  ${BSP_HIMAX_SRC}  
  ${BSP_HYPERFLASH_SRC}  
  ${BSP_NINA_SRC}  
  ${BSP_HYPERRAM_SRC}  
  ${BSP_SPIRAM_SRC}  
  ${BSP_SPIFLASH_SRC}  
  ${BSP_RAM_SRC}
)

set(GAPOC_A_SRC =  
  ${COMMON_SRC}  
  bsp/gapoc_a.c  
  ${BSP_CAMERA_SRC}  
  camera/mt9v034/mt9v034.c  
  ${BSP_HYPERFLASH_SRC}  
  transport/transport.c  
  transport/nina_w10/nina_w10.c  
  display/display.c  
  display/ili9341/ili9341.c  
  ${BSP_SPIRAM_SRC}  
  ${BSP_SPIFLASH_SRC}  
  ${BSP_HYPERRAM_SRC}  
  ${BSP_RAM_SRC}  
  ${BSP_BLE_NINA_B112_SRC}
)

if(${CHIP} STREQUAL "GAP8" AND ${TARGET_CHIP_VERSION} STREQUAL "1")
    set(GAPOC_B_SRC
        ${COMMON_SRC}  
        bsp/gapoc_b.c  
        ${BSP_HYPERFLASH_SRC}  
        display/display.c  
        display/ili9341/ili9341.c  
        ${BSP_HYPERRAM_SRC}  
        ${BSP_SPIRAM_SRC}  
        ${BSP_SPIFLASH_SRC}  
        ${BSP_RAM_SRC}  
        ${BSP_BLE_NINA_B112_SRC}  
        camera/pixart/pixart.c
        )
else()
    set(GAPOC_B_SRC
        ${COMMON_SRC}  
        bsp/gapoc_b_v2.c  
        ${BSP_CAMERA_SRC}  
        ${BSP_HYPERFLASH_SRC}  
        transport/transport.c  
        display/display.c  
        display/ili9341/ili9341.c  
        ${BSP_HYPERRAM_SRC}  
        ${BSP_SPIRAM_SRC}  
        ${BSP_SPIFLASH_SRC}  
        ${BSP_RAM_SRC}  
        ${BSP_BLE_NINA_B112_SRC}  
        camera/thermeye/thermeye.c  
        camera/ov5640/ov5640.c
        )
endif()				# TARGET_CHIP




if(${BOARD} STREQUAL "gapuino")
    set(C_SRCS ${GAPUINO_SRC})
elseif(${BOARD} STREQUAL "gapoc_a")
    set(C_SRCS ${GAPOC_A_SRC})
elseif(${BOARD} STREQUAL "gapoc_a_revb")
    set(C_SRCS ${GAPOC_A_SRC})
elseif(${BOARD} STREQUAL "gapoc_b")
    set(C_SRCS ${GAPOC_B_SRC})
elseif(${BOARD} STREQUAL "gapoc_b_revb")
    set(C_SRCS ${GAPOC_B_SRC})
elseif(${BOARD} STREQUAL "vega")
    set(C_SRCS ${VEGA_SRC})
elseif(${BOARD} STREQUAL "gap9_v2")
    set(C_SRCS ${GAP9_SRC})
elseif(${BOARD} STREQUAL "ai_deck")
    set(C_SRCS ${AI_DECK_SRC})
endif()

add_library(bsp STATIC ${C_SRCS})
target_link_libraries(bsp PRIVATE freertos)

target_include_directories(bsp PUBLIC include)

