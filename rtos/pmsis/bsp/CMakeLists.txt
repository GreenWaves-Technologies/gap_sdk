###############################################################################
# BOARD sources
###############################################################################

if(DEFINED CONFIG_BOARD_GAP9_EVK)
    LIST(APPEND BSP_SRCS  bsp/gap9_evk.c)
endif()

if(DEFINED CONFIG_BOARD_GAP9_EVK_AUDIO_ADDON)
    LIST(APPEND BSP_SRCS  bsp/gap9_evk_audio_addon.c)
endif()

if( DEFINED CONFIG_BOARD_GAP9_EVK
    OR CONFIG_BOARD_GAP9_EVK_AUDIO_ADDON)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAP9_EVK=1")
endif()

if(DEFINED CONFIG_BOARD_GAP9)
    LIST(APPEND BSP_SRCS  bsp/gap9.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAP9=1")
endif()

if(DEFINED CONFIG_BOARD_GAP9_V2)
    LIST(APPEND BSP_SRCS  bsp/gap9_v2.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAP9_V2=1")
endif()


if(DEFINED CONFIG_BOARD_GAPOC_A)
    LIST(APPEND BSP_SRCS  bsp/gapoc_a.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_A=1")
endif()

if(DEFINED CONFIG_BOARD_GAPOC_A_V2)
    LIST(APPEND BSP_SRCS  bsp/gapoc_a.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_A=1")
endif()

if(DEFINED CONFIG_BOARD_GAPOC_A_V2_SPI)
    LIST(APPEND BSP_SRCS  bsp/gapoc_a.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_A=1")
endif()

if(DEFINED CONFIG_BOARD_GAPOC_B)
    LIST(APPEND BSP_SRCS  bsp/gapoc_b.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_B=1")
endif()

if(DEFINED CONFIG_BOARD_GAPOC_B_V2)
    LIST(APPEND BSP_SRCS  bsp/gapoc_b.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_B=1")
endif()

if(DEFINED CONFIG_BOARD_GAPOC_B_V2_SPI)
    LIST(APPEND BSP_SRCS  bsp/gapoc_b.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPOC_B=1")
endif()

if(DEFINED CONFIG_BOARD_GAPUINO)
    LIST(APPEND BSP_SRCS  bsp/gapuino.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPUINO=1")
endif()

if(DEFINED CONFIG_BOARD_GAPUINO_V2)
    LIST(APPEND BSP_SRCS  bsp/gapuino.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPUINO=1")
endif()

if(DEFINED CONFIG_BOARD_GAPUINO_V2_SPI)
    LIST(APPEND BSP_SRCS  bsp/gapuino.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPUINO=1")
endif()

if(DEFINED CONFIG_BOARD_GAPUINO_V3)
    LIST(APPEND BSP_SRCS  bsp/gapuino.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPUINO=1")
endif()

if(DEFINED CONFIG_BOARD_GAPUINO_V3_SPI)
    LIST(APPEND BSP_SRCS  bsp/gapuino.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GAPUINO=1")
endif()

##############################################################################
# EEPROM sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_EEPROM_24XX1025)
    LIST(APPEND BSP_SRCS  eeprom/24XX1025.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_24XX1025=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_EEPROM_M24C02)
    LIST(APPEND BSP_SRCS  eeprom/m24c02.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_M24C02=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_EEPROM_VIRTUAL)
    LIST(APPEND BSP_SRCS  eeprom/virtual_eeprom.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_VIRTUAL_EEPROM=1")
endif()

###############################################################################
# FLASH sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_FLASH)
    LIST(APPEND BSP_SRCS  flash/flash.c)
endif()

if( DEFINED CONFIG_BSP_DRIVER_FLASH_SPIFLASH)
    LIST(APPEND BSP_SRCS  flash/spiflash/spiflash.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_SPIFLASH=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_FLASH_HYPERFLASH)
    LIST(APPEND BSP_SRCS  flash/hyperflash/hyperflash.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_HYPERFLASH=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_FLASH_MRAM)
    LIST(APPEND BSP_SRCS  flash/mram/mram-v2.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_MRAM=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_FLASH_ATXP032)
    LIST(APPEND BSP_SRCS  flash/spiflash/atxp032.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_ATXP032=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_FLASH_MX25U51245)
    LIST(APPEND BSP_SRCS  flash/spiflash/mx25u51245g.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_MX25U51245G=1")
endif()

###############################################################################
# RAM sources
###############################################################################

if( DEFINED CONFIG_BSP_DRIVER_RAM_HYPERRAM
    OR      CONFIG_BSP_DRIVER_RAM_SPIRAM_APS25XXXN)
  
    LIST(APPEND BSP_SRCS  ram/alloc_extern.c)
    LIST(APPEND BSP_SRCS  ram/ram.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_RAM_HYPERRAM)
    LIST(APPEND BSP_SRCS  ram/hyperram/hyperram.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_HYPERRAM=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_RAM_SPIRAM_APS25XXXN)
    LIST(APPEND BSP_SRCS  ram/spiram/aps25xxxn.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_APS25XXXN=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_RAM_SPIRAM)
    LIST(APPEND BSP_SRCS  ram/spiram/spiram.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_SPIRAM=1")
endif()

###############################################################################
# ADC sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_ADC_ADS1014)
    LIST(APPEND BSP_SRCS  adc/ads1014.c)
endif()

###############################################################################
# Audio sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_AUDIO_ADC_TLV320)
    LIST(APPEND BSP_SRCS  audio/adc/tlv320.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_TLV320=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_AUDIO_DAC_AK4332)
    LIST(APPEND BSP_SRCS  audio/dac/ak4332.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_AK4332=1")
endif()

###############################################################################
# BLE sources
###############################################################################

if( DEFINED CONFIG_BSP_DRIVER_BLE_NINA_B112
    OR      CONFIG_BSP_DRIVER_BLE_NINA_B312)
  
    LIST(APPEND BSP_SRCS  ble/ble.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_BLE_NINA_B112)
    # LIST(APPEND BSP_SRCS  ble/nina_b112/nina_b112_old.c)
    LIST(APPEND BSP_SRCS  ble/nina_b112/nina_b112.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_BLE_NINA_112=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_BLE_NINA_B312)
    LIST(APPEND BSP_SRCS  ble/nina_b312/nina_b312.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_BLE_NINA_312=1")
endif()

###############################################################################
# Camera sources
###############################################################################

if( DEFINED CONFIG_BSP_DRIVER_CAMERA_EPEAS_CIS001 
    OR      CONFIG_BSP_DRIVER_CAMERA_GC0308
    OR      CONFIG_BSP_DRIVER_CAMERA_HIMAX
    OR      CONFIG_BSP_DRIVER_CAMERA_HM0360
    OR      CONFIG_BSP_DRIVER_CAMERA_MT9V034
    OR      CONFIG_BSP_DRIVER_CAMERA_OV5640
    OR      CONFIG_BSP_DRIVER_CAMERA_OV5647
    OR      CONFIG_BSP_DRIVER_CAMERA_OV7670
    OR      CONFIG_BSP_DRIVER_CAMERA_OV9281
    OR      CONFIG_BSP_DRIVER_CAMERA_PIXART
    OR      CONFIG_BSP_DRIVER_CAMERA_THERMEYE)

    LIST(APPEND BSP_SRCS  camera/camera.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_EPEAS_CIS001)
    LIST(APPEND BSP_SRCS  camera/epeas_cis001/epeas_cis001.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_EPEAS_CIS001=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_GC0308)
    LIST(APPEND BSP_SRCS  camera/gc0308/gc0308.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_GC0308=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_HIMAX)
    LIST(APPEND BSP_SRCS  camera/himax/himax.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_HIMAX=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_HM0360)
    LIST(APPEND BSP_SRCS  camera/hm0360/hm0360.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_HM0360=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_MT9V034)
    LIST(APPEND BSP_SRCS  camera/mt9v034/mt9v034.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_MT9V034=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_OV5640)
    LIST(APPEND BSP_SRCS  camera/ov5640/ov5640.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_OV5640=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_OV5647)
    LIST(APPEND BSP_SRCS  camera/ov5647/ov5647.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_OV5647=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_OV7670)
    LIST(APPEND BSP_SRCS  camera/ov7670/ov7670.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_OV7670=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_OV9281)
    LIST(APPEND BSP_SRCS  camera/ov9281/ov9281.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_OV9281=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_PIXART)
    LIST(APPEND BSP_SRCS  camera/pixart/pixart.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_PIXART=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_CAMERA_THERMEYE)
    LIST(APPEND BSP_SRCS  camera/thermeye/thermeye.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_THERMEYE=1")
endif()

###############################################################################
# File system sources
###############################################################################

# FileSystem common sources
if( DEFINED CONFIG_BSP_DRIVER_FS_HOSTFS 
    OR      CONFIG_BSP_DRIVER_FS_LITTLEFS
    OR      CONFIG_BSP_DRIVER_FS_READFS)

    LIST(APPEND BSP_SRCS  fs/fs.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_FS_HOSTFS)
    LIST(APPEND BSP_SRCS fs/host_fs/host_fs.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_FS_LITTLEFS)
    LIST(APPEND BSP_SRCS fs/lfs/lfs_utils.c)
    LIST(APPEND BSP_SRCS fs/lfs/lfs.c)
    LIST(APPEND BSP_SRCS fs/lfs/pi_lfs.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_FS_READFS)
    LIST(APPEND BSP_SRCS fs/read_fs/read_fs.c)
endif()

###############################################################################
# GPIO sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_GPIO_FXL6408)
    LIST(APPEND BSP_SRCS gpio/fxl6408.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_FXL6408=1")
endif()

###############################################################################
# OTA sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_OTA)
    LIST(APPEND BSP_SRCS ota/ota.c)
    LIST(APPEND BSP_SRCS ota/ota_utility.c)
    LIST(APPEND BSP_SRCS ota/updater.c)
endif()

###############################################################################
# Bootloader sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_BOOTLOADER)
    LIST(APPEND BSP_SRCS  bootloader/bootloader_utility.c)
endif()

###############################################################################
# CRC sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_CRC_MD5)
    LIST(APPEND BSP_SRCS  crc/md5.c)
endif()

###############################################################################
# DISPLAYS sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_DISPLAY_ILI9341)
    LIST(APPEND BSP_SRCS  rtos/pmsis/bsp/display/ili9341/ili9341.c)
    LIST(APPEND BSP_SRCS  rtos/pmsis/bsp/display/display.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_ILI9341=1")
endif()

###############################################################################
# PARTITION sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_PARTITION)
    LIST(APPEND BSP_SRCS  partition/partition.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_FLASH_PARTITION)
    LIST(APPEND BSP_SRCS  partition/flash_partition.c)
endif()

###############################################################################
# TRANSPORT sources
###############################################################################

if(DEFINED CONFIG_BSP_DRIVER_TRANSPORT_NINA_W10
   OR      CONFIG_BSP_DRIVER_TRANSPORT_DA16200)
   LIST(APPEND BSP_SRCS  transport/transport.c)
endif()

if(DEFINED CONFIG_BSP_DRIVER_TRANSPORT_NINA_W10)
    LIST(APPEND BSP_SRCS  transport/nina_w10/nina_w10.c)
    LIST(APPEND BSP_SRCS  transport/nina_w10/firmware/main/nina_w10_fw.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_NINA_W10=1")
endif()

if(DEFINED CONFIG_BSP_DRIVER_TRANSPORT_DA16200)
    LIST(APPEND BSP_SRCS  transport/wifi/wifi.c)
    LIST(APPEND BSP_SRCS  transport/wifi/da16200/da16200.c)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DCONFIG_WIFI_DA16200=1")
endif()

if(DEFINED CONFIG_DEBUG_WIFI_DA16200)
    LIST(APPEND BSP_COMPILE_OPTIONS "-DDEBUG_DA16200=1")
    message(STATUS "Debug mode for DA16200 module activated")
endif()

###############################################################################
# Library Creation
###############################################################################

if(BSP_SRCS)
    # Create BSP Library including selected source files
    add_library(bsp STATIC ${BSP_SRCS})
    set(ENV{BSP_ENABLED} 1)

    # Add compilation options to bsp library
    target_compile_options(bsp PUBLIC ${BSP_COMPILE_OPTIONS})

    target_link_libraries(bsp PRIVATE freertos)
    target_include_directories(bsp PUBLIC include)
else()
    set(ENV{BSP_ENABLED} 0)
endif()

    message("check $ENV{CHECK}")
