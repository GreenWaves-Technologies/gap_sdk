#include "bsp/display/ili9341.h"
#include "bsp/camera/himax.h"
#include "bsp/camera/mt9v034.h"
#ifdef __FREERTOS__
#include "pmsis_api/include/rtos/pmsis_os.h"
#include "pmsis_api/include/rtos/os_frontend_api/pmsis_task.h"
#include "pmsis_api/include/rtos/pmsis_driver_core_api/pmsis_driver_core_api.h"
#include "pmsis_api/include/rtos/malloc/pmsis_l2_malloc.h"
#endif
#include "stdio.h"

#define QVGA 1
//#define QQVGA 1

#ifdef QVGA
#ifdef HIMAX
#define CAM_WIDTH    324
#define CAM_HEIGHT   244
#else
#define CAM_WIDTH    320
#define CAM_HEIGHT   240
#endif
#else
#define CAM_WIDTH    160
#define CAM_HEIGHT   120
#endif

#define LCD_WIDTH    320
#define LCD_HEIGHT   240

static pi_task_t task;
static unsigned char *imgBuff0;
static struct pi_device ili;
static pi_buffer_t buffer;
static struct pi_device device;
#ifdef USE_BRIDGE
static uint64_t fb;
#endif


static void lcd_handler(void *arg);
static void cam_handler(void *arg);


static void cam_handler(void *arg)
{
  camera_control(&device, CAMERA_CMD_STOP, 0);

#ifdef USE_BRIDGE
  rt_bridge_fb_update(fb, (unsigned int)imgBuff0, 0, 0, CAM_WIDTH, CAM_HEIGHT, NULL);
  camera_capture_async(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT, pi_task_callback(&task, cam_handler, NULL));
  camera_control(&device, CAMERA_CMD_START, 0);

#else
  #if 1
  printf("Received an image\n");
  //pmsis_exit(0);
  #else
  display_write_async(&ili, &buffer, 0, 0, LCD_WIDTH, LCD_HEIGHT, pi_task_callback(&task, lcd_handler, NULL));
  #endif
#endif
}


static void lcd_handler(void *arg)
{
  camera_control(&device, CAMERA_CMD_START, 0);
  camera_capture_async(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT, pi_task_callback(&task, cam_handler, NULL));

}


#ifdef USE_BRIDGE
static int open_bridge()
{
  rt_bridge_connect(1, NULL);

  fb = rt_bridge_fb_open("Camera", CAM_WIDTH, CAM_HEIGHT, RT_FB_FORMAT_GRAY, NULL);
  if (fb == 0) return -1;

  return 0;
}
#endif


static int open_display(struct pi_device *device)
{
#ifndef USE_BRIDGE
  struct ili9341_conf ili_conf;

  ili9341_conf_init(&ili_conf);

  pi_open_from_conf(device, &ili_conf);

  if (display_open(device))
    return -1;

  //if (display_ioctl(device, ILI_IOCTL_ORIENTATION, ILI_ORIENTATION_270))
  // return -1;

#else
  if (open_bridge())
  {
    printf("Failed to open bridge\n");
    return -1;
  }
#endif

  return 0;
}


static int open_camera_himax(struct pi_device *device)
{
  struct himax_conf cam_conf;

  himax_conf_init(&cam_conf);

#ifdef QVGA
  cam_conf.format = CAMERA_QVGA;
#endif

  pi_open_from_conf(device, &cam_conf);
  if (camera_open(device))
    return -1;

  return 0;
}

#define MT9V034_BLACK_LEVEL_CTRL  0x47
#define MT9V034_BLACK_LEVEL_AUTO  (0 << 0)
#define MT9V034_AEC_AGC_ENABLE    0xaf
#define MT9V034_AEC_ENABLE_A      (1 << 0)
#define MT9V034_AGC_ENABLE_A      (1 << 1)

static int open_camera_mt9v034(struct pi_device *device)
{
  struct mt9v034_conf cam_conf;

  mt9v034_conf_init(&cam_conf);

#ifdef QVGA
  cam_conf.format = CAMERA_QVGA;
#endif
#ifdef QQVGA
  cam_conf.format = CAMERA_QQVGA;
#endif

  pi_open_from_conf(device, &cam_conf);
  if (camera_open(device))
    return -1;

  uint16_t val = MT9V034_BLACK_LEVEL_AUTO;
  camera_reg_set(device, MT9V034_BLACK_LEVEL_CTRL, (uint8_t *) &val);
  val = MT9V034_AEC_ENABLE_A|MT9V034_AGC_ENABLE_A;
  camera_reg_set(device, MT9V034_AEC_AGC_ENABLE, (uint8_t *) &val);


  return 0;
}

static int open_camera(struct pi_device *device)
{
#ifdef GAPOC_A
  return open_camera_mt9v034(device);
#endif
#ifdef GAPUINO
  return open_camera_himax(device);
#endif
  return -1;
}

void test_ili9341_with_himax(void)
{
    printf("Entering main controller...\n");

#ifdef __PULP_OS__
  rt_freq_set(__RT_FREQ_DOMAIN_FC, 250000000);
#endif

  imgBuff0 = (unsigned char *)pmsis_l2_malloc((CAM_WIDTH*CAM_HEIGHT)*sizeof(unsigned char));
  if (imgBuff0 == NULL) {
      printf("Failed to allocate Memory for Image \n");
      pmsis_exit(1);
  }

  #ifndef __FREERTOS__
  if (open_display(&ili))
  {
    printf("Failed to open display\n");
    pmsis_exit(-1);
  }
  #endif

  if (open_camera(&device))
  {
    printf("Failed to open camera\n");
    pmsis_exit(-2);
  }

#ifdef HIMAX
  buffer.data = imgBuff0+CAM_WIDTH*2+2;
  buffer.stride = 4;

  // WIth Himax, propertly configure the buffer to skip boarder pixels
  pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0+CAM_WIDTH*2+2);
  pi_buffer_set_stride(&buffer, 4);
#else
  buffer.data = imgBuff0;
  pi_buffer_init(&buffer, PI_BUFFER_TYPE_L2, imgBuff0);
#endif
  pi_buffer_set_format(&buffer, CAM_WIDTH, CAM_HEIGHT, 1, PI_BUFFER_FORMAT_GRAY);

  while (1)
  {
      #if (ASYNC)
      camera_control(&device, CAMERA_CMD_STOP, 0);
      camera_capture_async(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT, pi_task_callback(&task, cam_handler, &device));
      camera_control(&device, CAMERA_CMD_START, 0);
      pi_task_wait_on(&task);
      #else
      printf("Camera start.\n");
      camera_control(&device, CAMERA_CMD_START, 0);
      camera_capture(&device, imgBuff0, CAM_WIDTH*CAM_HEIGHT);
      printf("Camera image captured.\n");
      camera_control(&device, CAMERA_CMD_STOP, 0);
      printf("Camera stop.\n");
      //pmsis_exit(0);
      #endif
  }
}

int main(void)
{
    printf("\n\t*** PMSIS ili9341 with himax test ***\n\n");
    return pmsis_kickoff((void *) test_ili9341_with_himax);
}
