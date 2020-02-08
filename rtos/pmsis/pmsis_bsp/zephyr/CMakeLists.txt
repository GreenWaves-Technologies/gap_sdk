zephyr_sources(
  ../camera/camera.c
  ../bsp/gapuino.c
  ../camera/camera.c
  ../camera/himax/himax.c
  ../display/display.c
  ../display/ili9341/ili9341.c
  ../fs/read_fs/read_fs.c
  ../fs/fs.c
  ../fs/host_fs/host_fs.c
  ../fs/host_fs/semihost.c
  ../flash/flash.c
  ../flash/hyperflash/hyperflash.c
  ../ram/ram.c
  ../ram/hyperram/hyperram.c
  ../ram/alloc_extern.c
)

zephyr_compile_options(
  -DCONFIG_GAPUINO
  )

zephyr_include_directories(../include)